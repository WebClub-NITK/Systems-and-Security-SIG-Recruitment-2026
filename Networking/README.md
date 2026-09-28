# Custom Security Layer (CSL): Build Your Own TLS-Inspired Protocol

## Introduction

Every secure connection on the internet relies on TLS, yet most engineers
never see what it does under the hood. In this task, you will design and
build your own secure protocol over raw TCP sockets — inspired by TLS
1.2's handshake and record layer — and use it to power an encrypted 1-1
chat application.

---

## Problem Statement

Design and implement **your own secure transport protocol** over raw TCP
sockets, and use it to build a **1-1 chat app**. This is not TLS 1.2
itself — no fixed wire format, no RFC compliance. Your protocol just
needs the same core guarantees: two parties with no prior shared secret
end a handshake with a symmetric key an eavesdropper cannot derive, and
every chat message after that is encrypted and tamper-evident.

Use any cryptographic library you like (OpenSSL, libsodium, a language's
standard crypto module) for primitives — DH, hashing, symmetric
encryption, MAC. You may not use a high-level secure-channel API (TLS
sockets, `ssl` module, `noise` library, etc.) — the handshake, framing,
and key derivation must be your own logic.

No certificates and no identity verification are required — assume both
sides are already correctly connected, and focus only on securing the
channel between them. The final deliverable: two users on separate
terminals/machines, running your client, who exchange encrypted messages
using keys established by a handshake at connection time — not hardcoded
or pre-shared.

Keep the codebase as well structured and neat as possible. Utilize comments to 
explain the code. Optionally, use a build system such as CMake.

Levels build on each other in order. Submit as far as you get, with
working code and a short writeup per completed level.

### Level 1 — TCP Plumbing and Message Framing

Set up a raw TCP client and server, and design your own message framing
(type byte, length field, payload) so structured messages can be
reassembled correctly, since TCP gives a byte stream, not message
boundaries. Prove it by exchanging a few test messages of different
types/lengths in both directions.

### Level 2 — Diffie-Hellman Key Exchange

Implement Diffie-Hellman as the key exchange algorithm:
each side generates a keypair, exchanges public values using your framing,
and independently computes the same shared secret. Confirm both sides
match without either copying the other's value.

(BONUS: Use Elliptic-Curve Diffie-Hellman instead of regular Diffie-Hellman)

### Level 3 — Key Derivation

Never use the raw DH secret directly as a key. Derive separate encryption
and MAC keys from it. The derivation need not match a standard KDF, but must
be a real function you implement — not truncation. 

(BONUS: Utilize an actual KDF like HKDF)

### Level 4 — Handshake Confirmation

Before any chat message is sent, have both sides prove the handshake
completed correctly — e.g., each computes a MAC over the handshake
transcript using a derived key, sends it, and the other verifies it
independently. Tamper with a public value in transit and confirm your
protocol detects it and aborts instead of continuing with mismatched
keys.

### Level 5 — Encrypted, Authenticated Messaging

Encrypt every chat message with a symmetric cipher and authenticate it
with a MAC (or use an AEAD mode like AES-GCM directly, and explain why a
separate MAC isn't needed).

(BONUS: Use a fresh nonce/IV per message, and include
a sequence number so replayed or reordered messages are detected, not
silently accepted.)

### Level 6 — The Chat Application

Wrap everything into a usable 1-1 chat app: the handshake runs
automatically on connect, then users type and receive messages via a
terminal, encrypted under the hood. 

### BONUS: Level 7 — Chat room

Extend your 1-1 chat into a multi-party chat room via a central server: each
client performs its own independent handshake with the server (its own DH exchange,
derived keys, and confirmation), so the server holds a separate session key per client.
On receiving a message from one client, the server decrypts and verifies it with that 
client's keys, then re-encrypts and forwards it to every other connected client under 
their own session keys and sequence numbers.

(BONUS: instead of a trusted relay, have all N parties derive one shared group key via
sequential DH, so the server never sees plaintext.)

### BONUS: Level 8 — Adding PKI

Your handshake currently authenticates nothing about identity — it only guarantees a
shared secret with whoever is on the other end of the socket, leaving it open to an active
MITM substituting their own DH value. Add long-term identity keypairs for each user, a minimal CA that signs
a binding of username to public key, and have each side sign its ephemeral DH public
value with its identity key so the peer can verify it against the CA-signed cert. 
Abort the handshake, as in Level 4, if either the cert or the DH-value signature fails to verify.

## Resources:
- [Transport Layer Security](https://www.geeksforgeeks.org/computer-networks/transport-layer-security-tls/)
- [Diffie-Hellman Key Exchange](https://www.geeksforgeeks.org/computer-networks/diffie-hellman-key-exchange-and-perfect-forward-secrecy/)
- [Elliptic Curve Diffie Hellman](https://medium.com/swlh/understanding-ec-diffie-hellman-9c07be338d4a)
- [Symmetric and Asymmetric Encryption](https://www.geeksforgeeks.org/computer-networks/difference-between-symmetric-and-asymmetric-key-encryption/)
- [TLS Handshake](https://www.cloudflare.com/learning/ssl/what-happens-in-a-tls-handshake/)
- [OpenSSL Documentation](https://docs.openssl.org/master/)
- [Libsodium Documentation](https://libsodium.gitbook.io/doc)
  
## Submission:
Create a private GitHub repo and add the mentors as collaborators.
- Nischay Bharadwaj Mahesh (PH: 9980543867, GH ID: N-tronics)
- Deepthi K (PH: 9448803285,GH ID: D-E-E-P-T-H-I)

In your repository,

Include a README.md with a detailed write-up of your protocol design, architecture and understanding of the project,along with any setbacks faced, debugging experiences and design decisions. 
 Your interest and efforts matter the most. Include screen recordings of the implementation at intermediate levels. Also include a final demonstration video. (You may also upload the video on Google Drive and share the link)
