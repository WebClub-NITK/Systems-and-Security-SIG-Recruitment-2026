# PulseBoard
Real-Time Web Subscriptions with Push Notifications — A Systems × GDG Task

#### Domain: Web Push API, Service Workers, Backend Polling, Concurrency & Scheduling

---

# Introduction

Modern browsers expose a powerful but often under-explored pipeline: **Web Push + Service Workers**. Together they let a web application deliver OS-level desktop notifications to a user even when the browser tab is closed — the same mechanism used by Gmail, Twitter, and native mobile apps.

**PulseBoard** is a self-hosted notification hub. Users subscribe to any web source — an RSS feed, a subreddit, a GitHub repository, a custom webhook — and receive a real desktop push notification the moment something new appears, without keeping a single tab open.

This task combines two engineering domains. On the **systems side**: concurrent polling schedulers, change-detection diffing, and reliable async delivery pipelines. On the **web side**: Service Worker lifecycle management, the Push API subscription flow, and VAPID authentication. Neither half works without the other.

You are not plugging into IFTTT or Firebase Cloud Messaging as a black box. You are building the plumbing yourself.

---

# Problem Statement

Design and implement **PulseBoard**: a full-stack web application that allows users to subscribe to web sources (RSS feeds and/or webhooks) and receive OS-level browser push notifications when new content is detected, using the **Web Push API** and **Service Workers**.

You will build the entire application from scratch:

- **Frontend** — a browser-based UI (plain HTML/JS, React, or any framework of your choice) that handles the Push API subscription flow and displays notification history.
- **Backend** — an HTTP API server (Node.js, Python, Go, or any language of your choice) that manages sources, runs the polling scheduler, and delivers push notifications via the Web Push protocol.
- **Database** — persist sources, push subscriptions, and notification history (SQLite, PostgreSQL, or equivalent).
- **Service Worker** (`sw.js`) — registered on the frontend; receives push events from the browser's push service and surfaces OS-level notifications even when the tab is closed.

The backend polling scheduler must run concurrently so that a slow or failing source does not delay others, and push delivery must be asynchronous so it does not block the scheduler.

---

# Tasks

## Phase 1 — Source Management

Implement the ability for users to add and remove **notification sources**. A source is one of:

- An **RSS / Atom feed URL** (e.g. a blog, a subreddit's `.rss`, a news site).
- A **webhook/JSON endpoint URL** (e.g. a GitHub releases API URL, or any HTTP endpoint whose response changes over time).

**Requirements:**
- A frontend page where users can add a source by entering a URL and a human-readable label, and remove existing ones.
- Backend REST API endpoints to `CREATE`, `LIST`, and `DELETE` sources, persisted to a database.
- The backend must validate that a submitted URL is reachable before saving it.
- In your `SUBMISSION.md`, describe the data model you chose and explain your schema decisions.

---

## Phase 2 — Concurrent Polling Scheduler

Implement a **backend scheduler** that periodically fetches every registered source and stores the latest snapshot.

**Requirements:**
- The scheduler must fetch all registered sources **concurrently** — one slow or failing source must not block others. Choose an explicit concurrency model (threads, async I/O, or a worker pool) and implement it intentionally.
- Each source is polled on a configurable interval (default: every 5 minutes).
- A timeout or network error on one source must be caught and logged without crashing the scheduler or skipping other sources.
- In your `SUBMISSION.md`, include timestamped logs showing that multiple sources are fetched concurrently (overlapping fetch timestamps), and explain the concurrency model you chose and why.

---

## Phase 3 — Change Detection

After fetching a source, the scheduler must determine whether anything **new** has appeared since the last poll.

**Requirements:**
- For **RSS feeds**: detect new entries by comparing item GUIDs or `<link>` values against a stored set of previously seen identifiers.
- For **JSON/webhook sources**: detect changes by hashing the response body (e.g. SHA-256) and comparing it against the stored hash, or by diffing a specific field the user configures (e.g. `$.tag_name`).
- Store a per-source fingerprint or last-seen snapshot in the database so that a server restart does not re-trigger all notifications.
- Only trigger a notification for **genuinely new** content — not on every successful poll.
- In your `SUBMISSION.md`, walk through one poll cycle where a source has new content and one where it does not, showing exactly what your code compares.

---

## Phase 4 — Service Worker & Push Subscription Pipeline

Set up the browser-side infrastructure that makes OS-level push notifications possible even when the tab is closed.

**Requirements:**
- Register a **Service Worker** (`sw.js`) from the frontend that correctly goes through the install → activate lifecycle.
- Implement the full **Push API subscription flow** in the frontend:
  1. Request notification permission from the user.
  2. Call `pushManager.subscribe(...)` with your server's **VAPID public key** to get a `PushSubscription` object.
  3. POST the `PushSubscription` (endpoint + `p256dh` + `auth` keys) to the backend and persist it in the database.
- The Service Worker's `push` event handler must parse the incoming payload and call `self.registration.showNotification(...)` with a title, body, icon, and a `data.url`.
- A `notificationclick` handler must open or focus the relevant content URL when the user clicks the notification.
- In your `SUBMISSION.md`, include a sequence diagram or step-by-step walkthrough of the full path: browser → push service → your backend → Service Worker → OS notification.

---

## Phase 5 — Server-Side Push Delivery

When Phase 3 detects new content, deliver a real push notification to all stored browser subscriptions.

**Requirements:**
- Use a Web Push library (`web-push` for Node.js, `pywebpush` for Python, or equivalent) to send VAPID-authenticated, encrypted push messages to each stored `PushSubscription` endpoint.
- The notification payload must include: the source label, a brief description of what changed, and a direct URL to the new content.
- If the push service responds with `410 Gone` or `404` for a subscription, remove it from the database — it is no longer valid.
- Push delivery must be kicked off **asynchronously** and must not block the polling scheduler from continuing to the next source.

---

## Phase 6 — Notification History Dashboard

Build a frontend dashboard that surfaces past notifications.

**Requirements:**
- A page listing all delivered notifications with: source name, timestamp, notification title, short description, and a link to the content.
- Notifications are shown as **read/unread**; clicking a notification marks it as read and the unread count updates accordingly.
- A visible unread badge in the navigation that reflects the current unread count (updated on load, or in real time).
- The Service Worker must persist incoming push payloads to **IndexedDB** so the notification history remains accessible even when offline.

---

## Phase 7 (Bonus) — Notification Preferences & Scheduling Controls

Add user-level controls over when and how notifications are delivered.

**Implement at least two of the following:**

- **Quiet hours**: suppress push delivery within a user-configured time window (e.g. 11 PM – 7 AM). Queue suppressed notifications and deliver them when quiet hours end.
- **Frequency limiting**: if the same source fires more than N notifications within an hour, batch them into a single digest push instead of N individual ones.
- **Digest mode**: instead of instant pushes, aggregate new items from a source and deliver a single summary at a user-set time (e.g. daily at 9 AM).
- **Per-source polling interval**: let users override the default poll interval per source (e.g. poll a GitHub repo every 2 minutes, a blog once per day).

---

## Phase 8 (Bonus) — Observability & Resilience

Implement at least two of the following to make the system more robust and inspectable:

- **Dead-letter store**: if a push delivery fails after N retries, write it to a dead-letter table or file for manual inspection rather than silently dropping it.
- **Metrics endpoint**: expose a `/metrics` route returning (at minimum) total active sources, polls in the last hour, total notifications sent, failed deliveries, and average poll latency per source.
- **Graceful shutdown**: on `SIGTERM`, let the scheduler finish all in-flight fetches and flush any pending push deliveries before exiting.
- **Outbound rate limiting**: cap the number of simultaneous outbound HTTP fetches (e.g. no more than 10 at once) to avoid hammering external servers.

---

# Submission

1. Create a **private** GitHub repository containing your complete implementation.
2. Keep the repository **private** until the submission deadline. Add all the project mentors as collaborators with read access.
3. Include a `SUBMISSION.md` in the root documenting:
   - **Setup instructions**: step-by-step commands to run the full stack locally, including environment variables, VAPID key generation, and database initialisation.
   - **Tech stack choices**: what language, framework, and database you used, and a brief rationale.
   - **Phase-by-phase notes**: for each attempted phase — design decisions, anything that surprised you, and evidence it works (logs, screenshots, or a short screen recording).
   - **Concurrency explanation** (Phase 2): which model you used and why.
   - **Change detection walkthrough** (Phase 3): before/after poll example with actual data.
   - **Push pipeline walkthrough** (Phase 4): sequence diagram or detailed step-by-step prose.
   - **Bonus phases**: if attempted, describe the approach and include any relevant output or screenshots.

---

# Resources

### Web Push & Service Workers
- [MDN: Using the Push API](https://developer.mozilla.org/en-US/docs/Web/API/Push_API)
- [MDN: Service Worker API](https://developer.mozilla.org/en-US/docs/Web/API/Service_Worker_API)
- [web-push npm library (Node.js)](https://github.com/web-push-libs/web-push)
- [pywebpush (Python)](https://github.com/web-push-libs/pywebpush)
- [VAPID: Voluntary Application Server Identification](https://datatracker.ietf.org/doc/html/rfc8292)
- [Google: Web Push Notifications Codelab](https://codelabs.developers.google.com/codelabs/push-notifications)

### RSS / Feed Parsing
- [feedparser (Python)](https://feedparser.readthedocs.io/en/latest/)
- [rss-parser (Node.js)](https://github.com/rbren/rss-parser)
- [RSS 2.0 Specification](https://www.rssboard.org/rss-specification)

### Concurrency & Scheduling
- [Python asyncio documentation](https://docs.python.org/3/library/asyncio.html)
- [APScheduler — Advanced Python Scheduler](https://apscheduler.readthedocs.io/)
- [Node.js: Worker Threads](https://nodejs.org/api/worker_threads.html)
- [node-cron](https://github.com/node-cron/node-cron)

### IndexedDB (Offline Storage)
- [MDN: IndexedDB API](https://developer.mozilla.org/en-US/docs/Web/API/IndexedDB_API)
- [idb — IDBs with Promises](https://github.com/jakearchibald/idb)

---

# Mentor's Details

1. Pari Tibrewal (+91 8240188219, GitHub: [pari1011](https://github.com/pari1011))
2. Lucky Verma (+91 9216932462, GitHub: [KALI-THE-HACKER](https://github.com/KALI-THE-HACKER))
