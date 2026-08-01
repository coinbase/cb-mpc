# CB-MPC (Coinbase Multi-Party Computation) Open Source Release

The [Coinbase HackerOne program](https://hackerone.com/coinbase) is the source of truth for the current bug bounty scope, eligibility requirements, and rewards. This document contains a copy of the relevant program content for convenience and may not reflect the latest updates.

Coinbase is proud to announce the open-sourcing of our MPC cryptography library! You can access it here: https://github.com/coinbase/cb-mpc. This significant milestone underscores our commitment to transparency, security, and promoting innovation within the cryptographic community.

With this release, we aim to:

* Enhance the security of the field by enabling developers to quickly deploy threshold signing/MPC for protecting cryptoassets in their applications.
* Increase transparency regarding Coinbase’s use of MPC, and encourage collaboration within the developer community.

Note that while the code is based on Coinbase's production environment, it is not exactly the same, and it has been modified to make it useful as a general-purpose library.

The primary focus of our bug bounty program will include identifying and addressing potential vulnerabilities in our open-source MPC implementation. Given the sensitive nature of these cryptographic protocols, it's imperative to safeguard against any exploits that could compromise cryptoassets. Responsible disclosure via the Bug Bounty Program or directly is encouraged (for direct disclosure see https://github.com/coinbase/cb-mpc/blob/master/SECURITY.md).

Through community collaboration and vigilant security reviews, we aspire to provide an easy to use and highly secure MPC library to help developers secure cryptoassets across the entire cryptocurrency and blockchain ecosystem.

To keep this bounty focused on issues that affect real integrations, eligible reports should target vulnerabilities reachable through the library's supported public APIs. High-level protocol entry points are exposed via the public C++ headers under `include/cbmpc/api/` (e.g., signing, DKG, TDH2).

For **High** and above, submissions must include a proof-of-concept that triggers the issue through those public APIs. Reports may reference or require fixes in `include-internal/` for root cause and impact analysis, but the PoC must not use `include-internal/` as the entry point. For MPC protocol-break PoCs, we expect the participating parties to run independently, ideally on separate machines. At least one honest party should use unmodified library code, and the malicious party should interact only through the protocol boundary exposed by the supported public APIs. Demo applications and sample code under `demo-*`, and the C API headers under `include/cbmpc/c_api/*`, are not in scope for this bug bounty program.

| Vulnerability Tier          | Description                           | Reward                                             |
|:-------------------|:--------------------------------|:------------------------------------------|
| **Extreme**      | Open Source Bugs (cb-mpc): not applicable | N/A |
| **Critical**      | Easily exploitable, high-severity bugs in protocols accessible from the public API (e.g., signing, DKG, TDH2 in the cb-mpc open-source library) that could lead to key compromise or remote code execution. Exploitation requires no material precondition beyond normal protocol operation and broadly affects typical deployments. | Up to $15,000 |
| **High**          | High-severity bugs in protocols accessible from the public API (e.g., signing, DKG, TDH2 in the cb-mpc open-source library) that could lead to key compromise or remote code execution, but require additional—yet realistic—conditions, access, or effort to exploit. Exploitation must remain feasible in a realistic deployment and must not depend on unlikely conditions. | Up to $6,000 |
| **Low / Medium** | Out of scope | $0 |
