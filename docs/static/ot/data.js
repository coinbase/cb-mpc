window.BENCHMARK_DATA = {
  "lastUpdate": 1761668684122,
  "repoUrl": "https://github.com/coinbase/cb-mpc",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "Arash-Afshar@users.noreply.github.com",
            "name": "Arash Afshar",
            "username": "Arash-Afshar"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3e545471b7e44a4f1858c6729184d6d4cccd6f50",
          "message": "feat: first release (#3)",
          "timestamp": "2025-03-21T10:42:58-06:00",
          "tree_id": "a14267dcc1c4572d5df58c559399a4a0ec47d107",
          "url": "https://github.com/coinbase/cb-mpc/commit/3e545471b7e44a4f1858c6729184d6d4cccd6f50"
        },
        "date": 1742577986140,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24440.720551724397,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24439.121482758626 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99063.5975713953,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99062.52885714288 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13775.621254897524,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13774.46854901961 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "Arash-Afshar@users.noreply.github.com",
            "name": "Arash Afshar",
            "username": "Arash-Afshar"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "dc7ce2156cfc61366a0f5f70761dacf22a35008b",
          "message": "chore: cleanup (#5)\n\n* fix: properly publish commit in benchmark page\n* feat: support go 1.24\n* chore: remove unused local variables",
          "timestamp": "2025-03-26T07:27:31-06:00",
          "tree_id": "ba26a2b6e0ce54b406c4d6ccc73b19506060313a",
          "url": "https://github.com/coinbase/cb-mpc/commit/dc7ce2156cfc61366a0f5f70761dacf22a35008b"
        },
        "date": 1742998249764,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24493.624071429364,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24491.654785714287 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99203.63271430686,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99173.97014285716 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13820.407823530724,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13819.068882352942 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d611643d608c17fadc7fa09793d00c84abfbaf94",
          "message": "feat: optimize Ed25519 point storage (#15)\n\n- Optimize Ed25519 point storage and operations for clarity and preparation for adding other curves.\n- Removed unused `set_coordinates` from `ecurve_interface_t`",
          "timestamp": "2025-05-05T22:41:02-07:00",
          "tree_id": "f6bbdb3391c7b9f77c66ab9d6afe21b91054bec3",
          "url": "https://github.com/coinbase/cb-mpc/commit/d611643d608c17fadc7fa09793d00c84abfbaf94"
        },
        "date": 1746512433952,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24442.108896562724,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24438.857103448274 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98865.46857140794,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98855.76185714288 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13782.67821569263,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13780.601333333329 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d96fee319fd9dc0820d8995be86a2f23ed1093f3",
          "message": "fix: RSA-OAEP MGF1 Misconfiguration (#16)\n\nPreviously, the RSA-OAEP implementation called the main hash setter twice instead of specifically setting the MGF1 hash. This caused MGF1 to incorrectly fall back to the default hash (SHA-1) instead of using the intended hash algorithm.",
          "timestamp": "2025-05-06T06:48:03-07:00",
          "tree_id": "d3ba2915cbb40e4c27ac2d51a315e7fb23cd2d17",
          "url": "https://github.com/coinbase/cb-mpc/commit/d96fee319fd9dc0820d8995be86a2f23ed1093f3"
        },
        "date": 1746541582532,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24376.948379320875,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24375.263206896558 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98937.16042857444,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98928.55142857145 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13817.080137262867,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13815.211078431374 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "225d67963edc510a9d3f83ce87fa780dbf256a25",
          "message": "fix: Zeroize plaintext on AES-GCM decryption failure (#17)\n\nPreviously, the AES-GCM `decrypt_final` function did not zeroize the decrypted plaintext from memory when the authentication tag verification failed. This could lead to sensitive data remaining in memory. We emphasize that none of the higher level protocols are impacted by this as they already zeroize the value. We are making this change for good hygiene.\n\nThis commit leverages `EVP_CIPHER_CTX` to ensure proper cleanup and zeroization of the plaintext buffer in the event of a decryption failure.",
          "timestamp": "2025-05-06T07:36:35-07:00",
          "tree_id": "597d1a2940de34c5960ba368686aa44c64bf65d7",
          "url": "https://github.com/coinbase/cb-mpc/commit/225d67963edc510a9d3f83ce87fa780dbf256a25"
        },
        "date": 1746544412902,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24375.38682758306,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24374.066689655174 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98951.68285714005,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98940.83085714285 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13791.27956862944,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13790.933686274508 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "4360c2097e506d7e51bd7a19c9a74b6344f254aa",
          "message": "chore: Address ASan and UBSan findings (#18)",
          "timestamp": "2025-05-07T07:11:09-06:00",
          "tree_id": "6d9e4e2097e973523bb3492d6467dbdd02cff8de",
          "url": "https://github.com/coinbase/cb-mpc/commit/4360c2097e506d7e51bd7a19c9a74b6344f254aa"
        },
        "date": 1746625700318,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24412.70886207537,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24409.450655172415 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98757.04599997854,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98748.3255714286 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13770.792921569213,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13768.312823529415 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "032abd771b188319cd4ee8c3e7663e8602132b97",
          "message": "chore: Add usage notes to core functions (#20)\n\nThis PR adds informational comments and/or updates documentation to clarify that many core functions intentionally omit boundary and overflow checks. This design was a deliberate choice to maximize performance in critical code paths.\n\nDevelopers using these functions must be aware that the responsibility for input validation and preventing overflow or out-of-bounds errors lies with the calling code. While this approach benefits performance, misusing these functions without proper external checks could potentially lead to security vulnerabilities if input is not carefully managed by the caller.\n\nThese changes are for documentation and awareness purposes only and do not alter the functional behavior of the core functions. The goal is to prevent accidental misuse and ensure developers understand the intentional performance-security trade-off in these specific areas.",
          "timestamp": "2025-05-07T06:55:07-07:00",
          "tree_id": "49ea819c770c958ba2d2caaf61af1ed6d6303803",
          "url": "https://github.com/coinbase/cb-mpc/commit/032abd771b188319cd4ee8c3e7663e8602132b97"
        },
        "date": 1746628457680,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24437.686517244558,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24434.66313793104 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98936.84357140436,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98921.182 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13822.155137252897,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13820.351529411762 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "1ce3207b3b3faecc210dca970f37a47061234eee",
          "message": "chore: Add benchmark for vartime curve operations (#21)",
          "timestamp": "2025-05-07T08:04:24-07:00",
          "tree_id": "7432baf39e3175583d0954415e1ac60801dc7e43",
          "url": "https://github.com/coinbase/cb-mpc/commit/1ce3207b3b3faecc210dca970f37a47061234eee"
        },
        "date": 1746632568094,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24503.79755172717,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24500.677482758623 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98875.76714286946,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98869.57857142857 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13772.67570588254,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13771.323254901967 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "9883f8dd9df8cae9d1c98c9dbee9fbecc9b61977",
          "message": "build: Use clang20 and clang‑format (#22)\n\n- Switch default compiler to Clang/Clang++ 20 and install clang‑format v20 in Docker\n  - Note that the necessity of `-Wno-error=deprecated-declarations` is because newer Clang surface the warning. The warning only appears in using google-test.\n- Reformat the entire codebase with clang‑format v20 (normalize lambda arrows, loop bodies, remove stray includes).\n- Reinforce constant‑time guarantees by inlining mask logic and inserting small assembly barriers in core modules. Note that some added barriers are necessary when we switch the compiler to Clang. The barriers prevent some optimizations that breaks the constant-time property.\n- Other minor changes: tidy up CMake options, `.gitignore`, and README.",
          "timestamp": "2025-05-07T09:46:19-07:00",
          "tree_id": "f2eb86e4d51a7656f8c948616d9c1ea305637822",
          "url": "https://github.com/coinbase/cb-mpc/commit/9883f8dd9df8cae9d1c98c9dbee9fbecc9b61977"
        },
        "date": 1746638655947,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24414.997000004125,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24414.240758620694 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98734.22757144518,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98729.91042857143 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13777.948960782227,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13776.380019607845 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "188008098+stepsecurity-app[bot]@users.noreply.github.com",
            "name": "stepsecurity-app[bot]",
            "username": "stepsecurity-app[bot]"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "245fe04db8aa92cfd13c5d9143d3380af40f5adc",
          "message": "[StepSecurity] Apply security best practices (#27)\n\nSigned-off-by: StepSecurity Bot <bot@stepsecurity.io>\nCo-authored-by: stepsecurity-app[bot] <188008098+stepsecurity-app[bot]@users.noreply.github.com>",
          "timestamp": "2025-06-13T07:35:38-07:00",
          "tree_id": "df9a8bf7fb70e122e8a652ea168a71ba0f89f859",
          "url": "https://github.com/coinbase/cb-mpc/commit/245fe04db8aa92cfd13c5d9143d3380af40f5adc"
        },
        "date": 1749827764382,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24631.514392857363,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24573.621821428573 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 100022.70214285351,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 100015.8732857143 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13773.373686276233,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13771.425901960782 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "047a698ada9a5dc4671ea9b63a767d43aa276725",
          "message": "chore: cleanup unused codes in buf128_t and buf256_t (#26)",
          "timestamp": "2025-06-14T09:38:51-07:00",
          "tree_id": "99a4ea4ac8f2945d771597aa4c902a6e9d6ce726",
          "url": "https://github.com/coinbase/cb-mpc/commit/047a698ada9a5dc4671ea9b63a767d43aa276725"
        },
        "date": 1749921462951,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24608.003103445484,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24607.513827586205 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99493.65257144954,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99483.22914285715 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13789.813339999455,
            "unit": "us/iter",
            "extra": "iterations: 50\ncpu: 13789.061740000008 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ee61f1f2ceb91985da7ff5f69d002b9cd8fdc6e8",
          "message": "fix: enable flexible t-of-n cryptogrpahic operations (#29)\n\n- Adds a missing feature in threshold dkg, where only parties 0-t could participate and no one else\n- Changes the job to accept party names (pname_t) as opposed to pids\n- Changes the go wrappers accordingly",
          "timestamp": "2025-06-17T09:27:04-06:00",
          "tree_id": "aaa6f62b84e3a69cb3441c51a2fa9979416abc5d",
          "url": "https://github.com/coinbase/cb-mpc/commit/ee61f1f2ceb91985da7ff5f69d002b9cd8fdc6e8"
        },
        "date": 1750176295515,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24500.77417857496,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24499.334857142854 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99268.2334285746,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99262.42142857143 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13802.508839999065,
            "unit": "us/iter",
            "extra": "iterations: 50\ncpu: 13802.416900000002 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "c7b470ad4d0d4cc313931317d202ffa5abe033b5",
          "message": "feat: Revamp Go wrapper (#31)\n\nAlso, we refactor the job_t's to facilitate the MPC protocol testing. In particular, it includes\n- introduces a data_transport_interface_t to decouple the MPC protocol logic from the underlying data transport mechanism. (Previously done in `job_session_t`, which is suboptimal)\n- The base job_mp_t class is refactored to use this interface, making it agnostic to the specific network implementation.\n- job_session_mp_t and its subclasses are simplified, bridging the generic job_mp_t with the parallel network_t.\n- Tests have been updated to reflect this new, more modular design.\n- Rename `job_session_t` to `job_parallel_t`\n\n* refactor: revamp Go wrapper\n\nNow the go wrapper conform with the standard Go convention,\nwith api, internal directories, etc.\nWe also introduce more wrappers as APIs around\n- curve\n- curve points\n- scalar\n- ECDSA-MP protocols\n- ECDSA-2P protocols\n- EdDSA-MP Protocols\n- ZK-DL Protocols",
          "timestamp": "2025-06-25T07:19:39-07:00",
          "tree_id": "1aae96b441550c242fe91717805c5fad5b547dc7",
          "url": "https://github.com/coinbase/cb-mpc/commit/c7b470ad4d0d4cc313931317d202ffa5abe033b5"
        },
        "date": 1750863422999,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24538.080678569586,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24537.82275 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99449.07485714795,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99445.00785714287 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13804.269686279084,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13803.859196078429 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "056e00cc1daba5e7bd6b4655a11920a9c4fd1cb9",
          "message": "feat: Add msg size to Full-OT bench (#34)\n\nAlso fixed the bug where Full-OT time benches were accmulative.",
          "timestamp": "2025-07-09T07:00:00-07:00",
          "tree_id": "3e652f80af54ea162f520ddc683fea75111ce6e0",
          "url": "https://github.com/coinbase/cb-mpc/commit/056e00cc1daba5e7bd6b4655a11920a9c4fd1cb9"
        },
        "date": 1752071931704,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24691.169392862583,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24690.50192857143 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 101379.3114285753,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 101372.15457142856 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13810.09496078025,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13809.47062745098 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "87906c7fd12c82d5fd999036dbe8610743f2c8aa",
          "message": "refactor: Crypto hardening and code cleanup (#36)\n\n- Removes numerous unused functions and code paths across the crypto modules (base, rsa, ec, ed25519).\n- Introduces input validation for deserializing containers (std::vector, bufs128_t) to guard against large allocation attacks.\n- Fixes an integer overflow vulnerability in the scr_inv modular inverse implementation.\n- Improves BN_CTX thread-local storage management by switching to scoped_ptr_t.\n- Strengthens the mod_t::coprime check by catching exceptions for non-invertible elements.\n- Adds a clear thread-safety warning to the main README.md.\n- Adds comprehensive unit tests for the coprime and scr_inv logic.",
          "timestamp": "2025-07-11T06:00:50-07:00",
          "tree_id": "8110280b666e0e9c1efc842ce35e49dbfa20eee5",
          "url": "https://github.com/coinbase/cb-mpc/commit/87906c7fd12c82d5fd999036dbe8610743f2c8aa"
        },
        "date": 1752241168945,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24505.776000004127,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24503.125000000004 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99270.55114284095,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99267.31857142856 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13848.423509800483,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13846.580705882352 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "cc4265d32ae59dab0c3917f7e39ac76fc44516a6",
          "message": "feat: Pluggable KEMs, Distributed Decryption, and Core Hardening (#45)\n\nThis commit introduces a major architectural refactor focused on cryptographic flexibility, API safety, and support for distributed operations. It incorporates several critical bug fixes related to concurrency, cryptographic correctness, and memory management. The public API has been updated, the CGO boundary has been hardened, and the developer experience has been improved.\n\n---\n## Key Architectural Changes\n\n### Pluggable, Instance-Based Architecture\nThe core API has been refactored from package-level functions to an **instance-based model**. Clients now create a handle (`NewPVE`) configured with a specific Key Encapsulation Mechanism (KEM) implementation. This is enabled by a new `mpc.KEM` Go interface and a generic `kem_aead_ciphertext_t` in C++, allowing different encryption backends to be plugged in seamlessly.\n\n### Interactive Distributed Decryption Protocol\nThe decryption process is now an **interactive, two-step protocol** for distributed environments, enhancing security by not requiring private keys to be co-located:\n1.  `AcPartyDecryptRow`: Each party generates a \"decryption share\" using its private key.\n2.  `AcAggregateToRestoreRow`: A new function aggregates shares from a quorum of parties to reconstruct the plaintext.\n\n### CGO Boundary Hardening & Memory Safety\nThe interface between Go and C++ has been overhauled to improve safety:\n* **No Leaked Handles**: The `go:linkname` pattern has been removed. Primitives (curve IDs, `[]byte`) are now passed instead of raw C++ pointers.\n* **Defensive Copying**: Data from Go is immediately copied into C++-owned buffers to prevent use-after-free errors from the Go GC (Fixes #42, #43).\n* **Memory Leak Fixed**: A memory leak in `hash32bit_for_zk_fischlin` was patched by correctly freeing the `EVP_MD_CTX` (Fixes #41).\n* **Clearer Ownership**: C++ message classes now own their data directly, improving clarity and memory safety.\n\n---\n## Additional Bug Fixes\n\n* **Deadlock Prevention**: A deadlock in parallel receive operations is now prevented. When a master thread encountered an error, it would leave worker threads blocked indefinitely. The fix ensures all waiting threads are properly notified on an error (Fixes #44).\n* **Corrected Public Key Reconstruction**: An uninitialized variable in the `reconstruct_pub_additive_shares` function could cause incorrect results. Shares are now correctly initialized to the curve's identity point (`infinity`), ensuring parties outside the quorum do not affect the sum (Fixes #38).\n* **ECDSA Test Fix**: The ECDSA test suite now works correctly with the `secp256k1` curve (Fixes #35).\n\n---\n## Developer Experience & Documentation\n\n* **Simplified Build Process**: A new `go_with_cpp.sh` wrapper script automatically rebuilds the C++ library as needed.\n* **Faster Filtered Tests**: The `make test-go` target is now significantly faster when using a filter.\n* **Cleaner Test Logs**: A new `testutil.TSilence` helper suppresses C++ `stderr` during tests that intentionally trigger errors.\n* **Documentation**: The documentation for Go wrapper compilation has been corrected.\n\nFixes #35, #38, #41, #42, #43, #44",
          "timestamp": "2025-09-25T07:20:22-07:00",
          "tree_id": "3f24c9802b3339b2fa1c1ee9fbe1802ac960ab45",
          "url": "https://github.com/coinbase/cb-mpc/commit/cc4265d32ae59dab0c3917f7e39ac76fc44516a6"
        },
        "date": 1758812363461,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24535.1983571384,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24531.386964285713 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98996.04185713023,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98991.07442857142 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13788.16329411968,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13787.261529411762 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "085d71b9928dc011a806af2cecb42af946374098",
          "message": "build: Improve OpenSSL build process and fix on macOS (#52)\n\nThis commit introduces several improvements to the build system, focusing on making the custom OpenSSL dependency easier to manage and fixing critical ordering issues in CMake.\n\nOpenSSL Build Improvements\nThe previous process for building the required custom OpenSSL was manual and error-prone. This change streamlines the entire experience.\n\nNew make openssl Target\nA new, platform-aware make openssl target has been added. It automatically detects the user's OS (macOS/Linux) and architecture (x86_64/ARM64) and runs the appropriate build script. This simplifies the setup instructions to a single command.\n\nCustomizable Install Path: The OpenSSL installation path was previously hardcoded. Now, users can specify a custom location by setting the CBMPC_OPENSSL_ROOT environment or CMake variable. This is crucial for environments where the user lacks write permissions to /usr/local/opt.\n\nCMake Fixes\nSeveral logical errors in the CMake configuration have been corrected to ensure a more reliable and deterministic build.",
          "timestamp": "2025-10-08T08:01:18-07:00",
          "tree_id": "d52df688bc731cf341ddc8532c399f476036fa54",
          "url": "https://github.com/coinbase/cb-mpc/commit/085d71b9928dc011a806af2cecb42af946374098"
        },
        "date": 1759938078915,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24430.830750004265,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24428.201392857143 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99157.96714286834,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99149.18800000001 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13803.640235294677,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13802.42260784313 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ec9a818f748e59ecec24676272d62e7e9068f6f1",
          "message": "fix: Prevent buffer overflow in converter (#54)\n\nAdd a bounds check in bits_t::convert to ensure there is sufficient data in the source buffer before deserializing. Also, add an assertion in converter_t::forward to protect against integer overflow on the offset.\n\nRemove the bufs128_t and bufs128_ref_t classes along with their associated functions, as they were not being used anywhere in the codebase.",
          "timestamp": "2025-10-09T11:02:02-07:00",
          "tree_id": "2157f87e79b77ff4fb1a01b419249988bf3e26e2",
          "url": "https://github.com/coinbase/cb-mpc/commit/ec9a818f748e59ecec24676272d62e7e9068f6f1"
        },
        "date": 1760035280522,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24512.37686207229,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24511.303862068966 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99221.21128569674,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99211.34742857143 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13786.215705883531,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13785.11988235294 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "231b02ec9224a5400473f344c31302caab741e7d",
          "message": "refactor: Consolidate threshold DKG logic into key_share_mp_t (#55)\n\nThe static methods for threshold distributed key generation (DKG) and key refresh have been moved from the `dkg_mp_threshold_t` struct to become static methods of `key_share_mp_t`.\n\nThis refactoring simplifies the code by removing the unnecessary `dkg_mp_threshold_t` struct and centralizes key generation logic within the key share structure itself.\n\nThe functions were renamed to `threshold_dkg` and `threshold_refresh` for clarity. All call sites and protocol wrappers (`ecdsa_mp`, `schnorr_mp`) have been updated to use the new API.",
          "timestamp": "2025-10-18T22:50:15-07:00",
          "tree_id": "820469edfdd766d9cc097b65b5b474fb780e0507",
          "url": "https://github.com/coinbase/cb-mpc/commit/231b02ec9224a5400473f344c31302caab741e7d"
        },
        "date": 1760855395476,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24386.587428564002,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24384.913892857145 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99493.57014284034,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99488.55742857141 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13831.29670588745,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13830.268156862745 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "50522e0ded8eefead310ba9c99513bd01751c7d6",
          "message": "fix: Prevent Out-of-Bounds read in PVE (#57)",
          "timestamp": "2025-10-27T08:13:03-07:00",
          "tree_id": "c54d31a69486f2e1673f2fb747f0cf27aaef3e32",
          "url": "https://github.com/coinbase/cb-mpc/commit/50522e0ded8eefead310ba9c99513bd01751c7d6"
        },
        "date": 1761580221251,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24062.730172412055,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24060.57603448276 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98185.86357141612,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98182.34228571427 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13716.060843138372,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13714.74311764706 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3aced93df47d96b405b84308252e4df314e68f6b",
          "message": "feat: Implement RFC 5869 HKDF and RFC 9180 DHKEM(P-256) (#58)\n\n- Add HKDF-Extract and HKDF-Expand functions per RFC 5869\n- Upgrade KEM-DEM to use HKDF for AES-GCM-256 key derivation\n- Implement RFC 9180 DHKEM(P-256, HKDF-SHA256) with labeled extract/expand\n- Add HPKE test vectors for P-256 KEM validation\n- Fix RSA OAEP label memory management to prevent invalid-free crashes",
          "timestamp": "2025-10-28T08:46:35-07:00",
          "tree_id": "8242d875446e59331df000a6293ab53d0602e49d",
          "url": "https://github.com/coinbase/cb-mpc/commit/3aced93df47d96b405b84308252e4df314e68f6b"
        },
        "date": 1761668683627,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24520.016714274916,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24519.045857142857 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99617.31271427458,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99612.17371428572 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13790.62278431543,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13789.599529411762 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}