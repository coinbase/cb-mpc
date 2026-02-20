# CB-MPC (Coinbase Multi-Party Computation) Open Source Release

Coinbase is proud to announce the open-sourcing of our MPC cryptography library! You can access it here: https://github.com/coinbase/cb-mpc. This significant milestone underscores our commitment to transparency, security, and promoting innovation within the cryptographic community.

With this release, we aim to:

* Enhance the security of the field by enabling developers to quickly deploy threshold signing/MPC for protecting cryptoassets in their applications.
* Increase transparency regarding Coinbase’s use of MPC, and encourage collaboration within the developer community.

Note that while the code is based on Coinbase's production environment, it is not exactly the same, and it has been modified to make it useful as a general-purpose library.

The primary focus of our bug bounty program will include identifying and addressing potential vulnerabilities in our open-source MPC implementation. Given the sensitive nature of these cryptographic protocols, it's imperative to safeguard against any exploits that could compromise cryptoassets. Responsible disclosure via the Bug Bounty Program or directly is encouraged (for direct disclosure see https://github.com/coinbase/cb-mpc/blob/master/SECURITY.md).

Through community collaboration and vigilant security reviews, we aspire to provide an easy to use and highly secure MPC library to help developers secure cryptoassets across the entire cryptocurrency and blockchain ecosystem.

To keep this bounty focused on issues that affect real integrations, eligible reports should target vulnerabilities reachable through the library's supported public APIs. High-level protocol entry points are exposed via the public C++ headers under `include/cbmpc/api/` (e.g., signing, DKG, TDH2).

For **Medium** and above, submissions must include a proof-of-concept that triggers the issue through those public APIs. Reports may reference or require fixes in `include-internal/` for root cause and impact analysis, but the PoC must not use `include-internal/` as the entry point. Demo applications and sample code under `demo-*`, and the C API headers under `include/cbmpc/c_api/*`, are not in scope for this bug bounty program.

| Vulnerability Tier          | Description                           | Reward                                             |
|:-------------------|:--------------------------------|:------------------------------------------|
| **Extreme**      | Open Source Bugs (cb-mpc): not applicable   |  Up to $1,000,000 | 
| **Critical**      | Open Source Bugs (cb-mpc): High-severity vulnerabilities in supported high-level protocols from the public API (e.g., Signing, DKG, TDH2) that are easily exploitable and can lead to key compromise. Examples: significant disclosure of sensitive data (key material), remote code execution. Private by default; triggers new releases for all supported versions.  | $50,000 | 
| **High**          | Open Source Bugs (cb-mpc): High-severity vulnerabilities in supported high-level protocols from the public API that are less easily exploitable. Private by default; triggers new release for all supported versions within a reasonable timeframe. | $15,000 |
| **Medium**        | Open Source Bugs (cb-mpc): Vulnerabilities that are hard to exploit, limited in impact, or present in less commonly used scenarios, but are still demonstrable via the supported public APIs. Bugs in lower-level cryptographic primitives (e.g., ZKPs, commitments) are eligible when reachable from those protocols. Private until next release; released with subsequent updates.            |  $2,000 |
| **Low**           | Open Source Bugs (cb-mpc):  Non-cryptographic issues including low-level non-cryptographic APIs, crashes, or deprecated cryptographic code. Any vulnerability in code that is released under “beta” is always low. Fixed immediately in latest development versions; may be backported to older versions.                              |  $200 | 
