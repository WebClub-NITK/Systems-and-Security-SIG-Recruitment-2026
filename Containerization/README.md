# Containerization
Full-Stack Application Containerization and Orchestration

#### Domain: Containers, Docker, Docker Compose, Nginx

# Introduction
Containerization is an operating-system-level virtualization method used to package an application alongside all its dependencies, runtime libraries, and configuration files into an isolated, predictable environment. Docker standardizes this process through container images and lightweight runtimes, eliminating differences between development, testing, and production environments. Docker Compose builds on Docker by providing a declarative YAML format to define, configure, and orchestrate multi-container application stacks with shared networking and volumes. In production architectures, services are typically placed behind a reverse proxy such as Nginx. Nginx acts as an intermediary that accepts external client requests, terminates SSL/TLS connections, routes traffic to internal application services, enforces rate limits, and distributes load across multiple backend instances. In this architecture, the frontend client communicates with the backend API through the reverse proxy rather than directly.

# Problem Statement
You are provided with a starter repository at [WebClub-NITK/systems-containerization-task-2026](https://github.com/WebClub-NITK/systems-containerization-task-2026) containing the codebase and an initial Docker, Docker Compose, and Nginx configuration for a full-stack web application (Node.js/Express backend, React/Vite/TailwindCSS frontend, and a PostgreSQL database with Prisma ORM). Your objective is to build upon this foundation to get the full stack running end-to-end, configure robust internal networking, establish an Nginx reverse proxy with SSL termination, implement load balancing across scaled service instances, and orchestrate the entire deployment via Docker Compose.

# Tasks

1. **Phase 1: End-to-End Build and Verification**  
   Get the full application stack building and running end-to-end using the Dockerfiles, Docker Compose configuration, and Nginx configuration provided in the repository. Verify that all components (database, backend API, frontend client, and reverse proxy) function correctly together. In your submission, document any unexpected behaviors, build errors, runtime errors, or incorrect responses encountered across any layer (application, container, or network). For every encountered issue, record the exact terminal or log output, how you diagnosed the root cause, and the changes made to resolve it.

2. **Phase 2: Frontend Image Optimization**  
   Inspect the provided frontend Dockerfile and review the resulting image size. If needed, optimize the Dockerfile so that the final frontend image remains strictly under 110MB. In your submission, detail how this size constraint was achieved or maintained.

3. **Phase 3: Database Configuration and Volume Persistence**  
   Ensure that the PostgreSQL database container is configured exclusively via environment variables (not hardcoded into application source code or Dockerfiles). The database port must remain internal to the container network and must not be published or exposed directly to the host machine. Ensure that database records persist across container destruction and restarts using a named Docker volume (`docker compose down` followed by `docker compose up`). Verify persistence by inserting test records, restarting the containers, and querying the data.

4. **Phase 4: Network Segmentation and Isolation**  
   Configure Docker networking to implement proper network segmentation: the frontend and reverse proxy sit on one network; the backend and database sit on a separate, isolated network that only the backend can reach; only the reverse proxy is exposed to the host on ports 80 and 443, and no other service ports are exposed to the host. Verify and, if needed, adjust your Docker Compose configuration to match this target architecture. Demonstrate this architecture in your submission with:
   - The complete output of `docker network inspect` for each defined network.
   - Verification tests (such as a host-level curl or port connection test proving that backend and database ports are unreachable from outside their designated network, contrasted with successful internal communication via `docker compose exec`).

5. **Phase 5: Nginx SSL Termination and HTTPS Redirection**  
   Extend the provided Nginx configuration to support secure HTTPS connections. Generate and configure SSL certificates (either self-signed certificates or Let's Encrypt / Certbot). Configure Nginx to terminate SSL on port 443, proxy decrypted traffic to the frontend and backend services, and automatically redirect all incoming HTTP port 80 traffic to HTTPS.

6. **Phase 6: Consolidated Orchestration and Build Cache Analysis**  
   Finalize your setup into a clean, reproducible `docker-compose.yml` file supporting single-command startup (`docker compose up --build`). In this phase, extract any inline or hardcoded configuration values and secrets from the Compose file into an environment file (`.env` or equivalent secret-handling approach). Once working, modify a single line of application source code (in either frontend or backend) and execute a rebuild. In your submission, analyze the build log and explain which Docker build layers resulted in cache hits versus rebuilds, detailing how Dockerfile instruction ordering influences caching efficiency.

7. **Phase 7: Bonus — Scaling, Load Balancing, and Rate Limiting**  
   Scale the backend service to run multiple container instances (e.g., via Docker Compose replica settings). Update the Nginx configuration to define an `upstream` pool and load balance incoming `/api/` traffic across the backend replicas. Implement rate limiting in Nginx to protect the API endpoints against burst traffic. Run an HTTP load test (using tools such as `wrk`, `autocannon`, or `ab`) against a public, unauthenticated endpoint of your choice, comparing a single backend instance against the scaled setup. In your submission, paste the complete raw output from both benchmark runs and provide an analysis explaining the observed performance differences.

8. **Phase 8: Bonus — Container Hardening and CI Pipeline**  
   Enhance container security by configuring non-root users (`USER`) inside the Dockerfiles, ensuring appropriate `.dockerignore` files prevent local files and secrets from leaking into build contexts, and removing unnecessary packages or build tools from runtime images. Create a minimal GitHub Actions workflow (`.github/workflows/docker-build.yml`) that triggers on push to build and lint the Docker images.

# Submission

1. Create a **private** GitHub repository containing your complete configuration files, Dockerfiles, compose definitions, and source changes.
2. Maintain the repository as **private** until explicitly requested by mentors to change its visibility. Add [KALI-THE-HACKER](https://github.com/KALI-THE-HACKER), [antonyth18](https://github.com/antonyth18) as collaborators with read access.
3. Include a comprehensive `SUBMISSION.md` in the root of your repository documenting:
   - Clear step-by-step reproduction instructions to start the entire stack.
   - For Phase 1, full diagnostic notes for any unexpected behavior encountered, including raw logs, diagnosis, and fixes applied.
   - Raw output, logs, and screenshots wherever explicitly requested:
     - Optimization rationale and final image sizes (`docker images`) for Phase 2.
     - Verification of database persistence and environment configuration for Phase 3.
     - Raw output of `docker network inspect` and network isolation tests for Phase 4.
     - SSL configuration and redirection verification for Phase 5.
     - Docker layer cache hit/miss analysis with build logs for Phase 6.
     - If attempting bonus phases: complete raw output comparing single-instance vs. scaled load tests for Phase 7, and evidence of non-root container configuration (e.g. `docker exec ... whoami`) plus a screenshot of a successful GitHub Actions run for Phase 8.

# Resources

- [Docker Getting Started Tutorial](https://docs.docker.com/get-started/)
- [Docker Compose Overview and Networking](https://docs.docker.com/compose/networking/)
- [Nginx Beginner's Guide](https://nginx.org/en/docs/beginners_guide.html)
- [Docker Multi-Stage Builds Documentation](https://docs.docker.com/build/building/multi-stage/)
- [Let's Encrypt / Certbot Documentation](https://certbot.eff.org/)
- [wrk — Modern HTTP Benchmarking Tool](https://github.com/wg/wrk)
- [GitHub Actions: Publishing Docker Images](https://docs.github.com/en/actions/publishing-packages/publishing-docker-images)
