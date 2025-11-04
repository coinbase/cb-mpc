window.BENCHMARK_DATA = {
  "lastUpdate": 1762279053318,
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
        "date": 1742577979621,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3588439336388016,
            "unit": "us/iter",
            "extra": "iterations: 1953733\ncpu: 0.35880302426175936 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.2466610755483898,
            "unit": "us/iter",
            "extra": "iterations: 2839742\ncpu: 0.24664440818919464 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.35152947080926,
            "unit": "us/iter",
            "extra": "iterations: 16932\ncpu: 41.3489343845972 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 49.55141816765529,
            "unit": "us/iter",
            "extra": "iterations: 14102\ncpu: 49.55022493263366 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.90506365526782,
            "unit": "us/iter",
            "extra": "iterations: 35111\ncpu: 19.904156076443293 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 7.6298574360626725,
            "unit": "us/iter",
            "extra": "iterations: 92085\ncpu: 7.62944603355596 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.74237612017843,
            "unit": "us/iter",
            "extra": "iterations: 11382\ncpu: 61.723977947636605 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 57.21650626277122,
            "unit": "us/iter",
            "extra": "iterations: 12215\ncpu: 57.214875317232924 us\nthreads: 1"
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
        "date": 1742998241748,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3594652520810311,
            "unit": "us/iter",
            "extra": "iterations: 1939555\ncpu: 0.3594433475719946 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.24628261159251752,
            "unit": "us/iter",
            "extra": "iterations: 2844165\ncpu: 0.2462554127485571 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.50078333630018,
            "unit": "us/iter",
            "extra": "iterations: 16911\ncpu: 41.498773815859494 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 50.289644999265356,
            "unit": "us/iter",
            "extra": "iterations: 13938\ncpu: 50.286433562921516 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.97770915162169,
            "unit": "us/iter",
            "extra": "iterations: 35032\ncpu: 19.977604675725054 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 7.576458801723659,
            "unit": "us/iter",
            "extra": "iterations: 91715\ncpu: 7.575113024041868 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.71505729120839,
            "unit": "us/iter",
            "extra": "iterations: 11363\ncpu: 61.7036779899674 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 57.54031508988265,
            "unit": "us/iter",
            "extra": "iterations: 12187\ncpu: 57.537597357840305 us\nthreads: 1"
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
        "date": 1746512426404,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3590930804775091,
            "unit": "us/iter",
            "extra": "iterations: 1948357\ncpu: 0.35905800887619665 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.31242825936092766,
            "unit": "us/iter",
            "extra": "iterations: 2242188\ncpu: 0.31239650689415877 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.389981993150734,
            "unit": "us/iter",
            "extra": "iterations: 16938\ncpu: 41.38704675876727 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 64.04800347158958,
            "unit": "us/iter",
            "extra": "iterations: 10946\ncpu: 64.0417280285036 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.012877776824645,
            "unit": "us/iter",
            "extra": "iterations: 35067\ncpu: 20.01097604585508 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.217622712484651,
            "unit": "us/iter",
            "extra": "iterations: 49180\ncpu: 14.216336376575848 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.54081536565748,
            "unit": "us/iter",
            "extra": "iterations: 11363\ncpu: 61.53464340403058 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.47859452791319,
            "unit": "us/iter",
            "extra": "iterations: 8918\ncpu: 78.47068995290424 us\nthreads: 1"
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
        "date": 1746541576748,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.36166999167088304,
            "unit": "us/iter",
            "extra": "iterations: 1953381\ncpu: 0.361610810691821 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.314314471175581,
            "unit": "us/iter",
            "extra": "iterations: 2227034\ncpu: 0.3142947763707244 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.416874719167375,
            "unit": "us/iter",
            "extra": "iterations: 16914\ncpu: 41.410657502660534 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 64.0886724783111,
            "unit": "us/iter",
            "extra": "iterations: 10955\ncpu: 64.08313783660428 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.016765598655457,
            "unit": "us/iter",
            "extra": "iterations: 34987\ncpu: 20.01419947409039 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.194526681994905,
            "unit": "us/iter",
            "extra": "iterations: 49584\ncpu: 14.19343852855759 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.51589256858534,
            "unit": "us/iter",
            "extra": "iterations: 11263\ncpu: 61.50832025215305 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.60117100245883,
            "unit": "us/iter",
            "extra": "iterations: 8918\ncpu: 78.59288035433954 us\nthreads: 1"
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
        "date": 1746544406469,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35881598824308625,
            "unit": "us/iter",
            "extra": "iterations: 1949495\ncpu: 0.3588006365751131 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.31181699984840366,
            "unit": "us/iter",
            "extra": "iterations: 2242020\ncpu: 0.3118153905852759 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.42955783317903,
            "unit": "us/iter",
            "extra": "iterations: 16928\ncpu: 41.428252894612456 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.95458845028939,
            "unit": "us/iter",
            "extra": "iterations: 10944\ncpu: 63.95322514619883 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.947376835561244,
            "unit": "us/iter",
            "extra": "iterations: 35071\ncpu: 19.944560434547057 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.541858203341516,
            "unit": "us/iter",
            "extra": "iterations: 49169\ncpu: 14.539804063535978 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.57763698689941,
            "unit": "us/iter",
            "extra": "iterations: 11377\ncpu: 61.57040722510323 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.51862830866406,
            "unit": "us/iter",
            "extra": "iterations: 8916\ncpu: 78.51182133243597 us\nthreads: 1"
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
        "date": 1746625693639,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35934800506326825,
            "unit": "us/iter",
            "extra": "iterations: 1949736\ncpu: 0.35932010487573696 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3152531979287471,
            "unit": "us/iter",
            "extra": "iterations: 2219718\ncpu: 0.3152110394203227 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.343136113569905,
            "unit": "us/iter",
            "extra": "iterations: 16905\ncpu: 41.34013203194319 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 66.54846771130839,
            "unit": "us/iter",
            "extra": "iterations: 10530\ncpu: 66.5416237416904 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.978690652200797,
            "unit": "us/iter",
            "extra": "iterations: 35035\ncpu: 19.976642643071223 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.25367599401785,
            "unit": "us/iter",
            "extra": "iterations: 49496\ncpu: 14.252846977533542 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.6959098143323,
            "unit": "us/iter",
            "extra": "iterations: 11310\ncpu: 61.69075083996465 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.96647624423672,
            "unit": "us/iter",
            "extra": "iterations: 8861\ncpu: 78.9559749463943 us\nthreads: 1"
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
        "date": 1746628451867,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35942039301781975,
            "unit": "us/iter",
            "extra": "iterations: 1946833\ncpu: 0.35936988175154205 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3155555692305305,
            "unit": "us/iter",
            "extra": "iterations: 2218143\ncpu: 0.3155232187464921 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 42.468925783563925,
            "unit": "us/iter",
            "extra": "iterations: 16910\ncpu: 42.466733707865146 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 65.5988493074124,
            "unit": "us/iter",
            "extra": "iterations: 10757\ncpu: 65.59133178395463 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.96368800685594,
            "unit": "us/iter",
            "extra": "iterations: 35020\ncpu: 19.961354997144483 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.107878043866933,
            "unit": "us/iter",
            "extra": "iterations: 49608\ncpu: 14.106667876149013 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.650142026441735,
            "unit": "us/iter",
            "extra": "iterations: 11350\ncpu: 61.643958502202636 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.77934213780394,
            "unit": "us/iter",
            "extra": "iterations: 8897\ncpu: 78.77394818478132 us\nthreads: 1"
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
        "date": 1746632561790,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3571312565255099,
            "unit": "us/iter",
            "extra": "iterations: 1963430\ncpu: 0.35710012019781706 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.31446234983597765,
            "unit": "us/iter",
            "extra": "iterations: 2229645\ncpu: 0.314458108353572 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.3445838324428843,
            "unit": "us/iter",
            "extra": "iterations: 2031451\ncpu: 0.34456463138909077 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.31388434207739335,
            "unit": "us/iter",
            "extra": "iterations: 2230232\ncpu: 0.31383570005272987 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.35523796016765,
            "unit": "us/iter",
            "extra": "iterations: 16923\ncpu: 41.35319700998642 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.73167042253492,
            "unit": "us/iter",
            "extra": "iterations: 11005\ncpu: 63.73087278509774 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.458354692649525,
            "unit": "us/iter",
            "extra": "iterations: 21864\ncpu: 31.456556759970706 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.793089162106526,
            "unit": "us/iter",
            "extra": "iterations: 10980\ncpu: 63.79107750455374 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.95771035072828,
            "unit": "us/iter",
            "extra": "iterations: 35070\ncpu: 19.955209609352725 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.340004821105518,
            "unit": "us/iter",
            "extra": "iterations: 48744\ncpu: 14.338682135237178 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 19.965464092183677,
            "unit": "us/iter",
            "extra": "iterations: 35062\ncpu: 19.96393825223885 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.302714416875869,
            "unit": "us/iter",
            "extra": "iterations: 95846\ncpu: 7.302280491621991 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.661143661475236,
            "unit": "us/iter",
            "extra": "iterations: 11367\ncpu: 61.65565487815603 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.9491835214382,
            "unit": "us/iter",
            "extra": "iterations: 8860\ncpu: 78.94563611738141 us\nthreads: 1"
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
        "date": 1746638650148,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35720033502247067,
            "unit": "us/iter",
            "extra": "iterations: 1957481\ncpu: 0.35717767835294434 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.30733516848807985,
            "unit": "us/iter",
            "extra": "iterations: 2274286\ncpu: 0.30731176070204014 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.3458599299086729,
            "unit": "us/iter",
            "extra": "iterations: 2025643\ncpu: 0.3458307095574098 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.30746335237031,
            "unit": "us/iter",
            "extra": "iterations: 2161927\ncpu: 0.3074505998583673 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.370456725942006,
            "unit": "us/iter",
            "extra": "iterations: 16927\ncpu: 41.367861227624516 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 65.15788922934176,
            "unit": "us/iter",
            "extra": "iterations: 10770\ncpu: 65.15250167130917 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.4519914967509,
            "unit": "us/iter",
            "extra": "iterations: 21874\ncpu: 32.449615296699285 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 65.05501907153382,
            "unit": "us/iter",
            "extra": "iterations: 10749\ncpu: 65.04812196483387 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.986185142985146,
            "unit": "us/iter",
            "extra": "iterations: 35108\ncpu: 19.985175116782536 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.286585785855253,
            "unit": "us/iter",
            "extra": "iterations: 49303\ncpu: 14.286274222663923 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.073166895369727,
            "unit": "us/iter",
            "extra": "iterations: 34980\ncpu: 20.072131532304205 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.338101183234641,
            "unit": "us/iter",
            "extra": "iterations: 95332\ncpu: 7.337674925523443 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.66619522929734,
            "unit": "us/iter",
            "extra": "iterations: 11361\ncpu: 61.66344828800295 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 79.73660282234948,
            "unit": "us/iter",
            "extra": "iterations: 8787\ncpu: 79.7269016729259 us\nthreads: 1"
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
        "date": 1749827757191,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35761584573979815,
            "unit": "us/iter",
            "extra": "iterations: 1958924\ncpu: 0.35758429882935727 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.307698962671129,
            "unit": "us/iter",
            "extra": "iterations: 2274110\ncpu: 0.307670896306687 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.3453807884639972,
            "unit": "us/iter",
            "extra": "iterations: 2027740\ncpu: 0.3453444218686813 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3075518714399871,
            "unit": "us/iter",
            "extra": "iterations: 2277284\ncpu: 0.3075128723514502 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.373936116040205,
            "unit": "us/iter",
            "extra": "iterations: 16890\ncpu: 41.36987205447012 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 64.97170035259752,
            "unit": "us/iter",
            "extra": "iterations: 10776\ncpu: 64.96409511878247 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.695043260612806,
            "unit": "us/iter",
            "extra": "iterations: 21775\ncpu: 32.69420734787598 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 65.11002838794892,
            "unit": "us/iter",
            "extra": "iterations: 10744\ncpu: 65.10639798957558 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.949875909264556,
            "unit": "us/iter",
            "extra": "iterations: 34918\ncpu: 19.948474884013994 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.258104932129589,
            "unit": "us/iter",
            "extra": "iterations: 49289\ncpu: 14.2570397857534 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.00622644627573,
            "unit": "us/iter",
            "extra": "iterations: 35090\ncpu: 20.00489871758335 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 8.901504148426534,
            "unit": "us/iter",
            "extra": "iterations: 78584\ncpu: 8.900957650412296 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.704406266523804,
            "unit": "us/iter",
            "extra": "iterations: 11362\ncpu: 61.702184034500974 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 79.68670348040744,
            "unit": "us/iter",
            "extra": "iterations: 8792\ncpu: 79.68474101455872 us\nthreads: 1"
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
        "date": 1749921456197,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.362755908197711,
            "unit": "us/iter",
            "extra": "iterations: 1958296\ncpu: 0.36205318092872574 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3121093102132305,
            "unit": "us/iter",
            "extra": "iterations: 2241968\ncpu: 0.3120738538641051 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.34561452233076956,
            "unit": "us/iter",
            "extra": "iterations: 2020750\ncpu: 0.34556852752690825 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3119577597401599,
            "unit": "us/iter",
            "extra": "iterations: 2239901\ncpu: 0.3119470311411084 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.58972069824928,
            "unit": "us/iter",
            "extra": "iterations: 16842\ncpu: 41.585392471202944 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.505884171298185,
            "unit": "us/iter",
            "extra": "iterations: 10740\ncpu: 63.50181918063314 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.051539360593218,
            "unit": "us/iter",
            "extra": "iterations: 21989\ncpu: 31.05024093865115 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.57781293070031,
            "unit": "us/iter",
            "extra": "iterations: 11028\ncpu: 63.57459022488215 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.055058401059274,
            "unit": "us/iter",
            "extra": "iterations: 34948\ncpu: 20.05384826027239 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.335969346552364,
            "unit": "us/iter",
            "extra": "iterations: 49032\ncpu: 14.33431536547561 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.08364240487606,
            "unit": "us/iter",
            "extra": "iterations: 34746\ncpu: 20.08264672192484 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.273376366660183,
            "unit": "us/iter",
            "extra": "iterations: 96220\ncpu: 7.272568935772177 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 62.128430671162846,
            "unit": "us/iter",
            "extra": "iterations: 11294\ncpu: 62.123108199043706 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.5155046551118,
            "unit": "us/iter",
            "extra": "iterations: 8915\ncpu: 78.50964991587212 us\nthreads: 1"
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
        "date": 1750176288655,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35690557848108323,
            "unit": "us/iter",
            "extra": "iterations: 1949312\ncpu: 0.35689429603880757 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.30846159764322634,
            "unit": "us/iter",
            "extra": "iterations: 2266814\ncpu: 0.3084445772789474 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.34727329890758873,
            "unit": "us/iter",
            "extra": "iterations: 2021157\ncpu: 0.34725826890241585 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.31023865955064633,
            "unit": "us/iter",
            "extra": "iterations: 2266643\ncpu: 0.31021923081843944 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.46737181007613,
            "unit": "us/iter",
            "extra": "iterations: 16027\ncpu: 41.46482729144566 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.87866903267183,
            "unit": "us/iter",
            "extra": "iterations: 10989\ncpu: 63.876972699972704 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.41782148150963,
            "unit": "us/iter",
            "extra": "iterations: 22261\ncpu: 32.41708207178473 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.73825483432422,
            "unit": "us/iter",
            "extra": "iterations: 11015\ncpu: 63.73512274171582 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.990886287529683,
            "unit": "us/iter",
            "extra": "iterations: 34939\ncpu: 19.990488737513946 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.09199777296502,
            "unit": "us/iter",
            "extra": "iterations: 48944\ncpu: 14.091245014710683 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.10513991108097,
            "unit": "us/iter",
            "extra": "iterations: 34865\ncpu: 20.104632439409134 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.325241529907614,
            "unit": "us/iter",
            "extra": "iterations: 95483\ncpu: 7.324845700281717 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.705959985831804,
            "unit": "us/iter",
            "extra": "iterations: 11296\ncpu: 61.703373229461796 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.5716844051256,
            "unit": "us/iter",
            "extra": "iterations: 8926\ncpu: 78.56717902755993 us\nthreads: 1"
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
        "date": 1750863415288,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35725776735257786,
            "unit": "us/iter",
            "extra": "iterations: 1952113\ncpu: 0.3572557731032988 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3132127507607672,
            "unit": "us/iter",
            "extra": "iterations: 2237341\ncpu: 0.31317985725019115 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.34549501994704745,
            "unit": "us/iter",
            "extra": "iterations: 2018854\ncpu: 0.34547769774337317 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.31275722106863546,
            "unit": "us/iter",
            "extra": "iterations: 2244647\ncpu: 0.31273218060568114 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.332841267030375,
            "unit": "us/iter",
            "extra": "iterations: 16890\ncpu: 41.33100254588514 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.35695469723043,
            "unit": "us/iter",
            "extra": "iterations: 11081\ncpu: 63.34631621694792 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.83169524064538,
            "unit": "us/iter",
            "extra": "iterations: 22503\ncpu: 32.82892512109499 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.39972231780198,
            "unit": "us/iter",
            "extra": "iterations: 11045\ncpu: 63.39453309189679 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.086116770504624,
            "unit": "us/iter",
            "extra": "iterations: 34829\ncpu: 20.085396795773626 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.337530313814689,
            "unit": "us/iter",
            "extra": "iterations: 49169\ncpu: 14.335134556326121 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.085425106216327,
            "unit": "us/iter",
            "extra": "iterations: 34836\ncpu: 20.084391261912966 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.293013325986428,
            "unit": "us/iter",
            "extra": "iterations: 96203\ncpu: 7.292496086400639 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.790936763402875,
            "unit": "us/iter",
            "extra": "iterations: 11370\ncpu: 61.78435540897098 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.07285486568679,
            "unit": "us/iter",
            "extra": "iterations: 8971\ncpu: 78.06616709396953 us\nthreads: 1"
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
        "date": 1752071924486,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.37080989564869726,
            "unit": "us/iter",
            "extra": "iterations: 1931839\ncpu: 0.3707859433420694 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.31012532357275974,
            "unit": "us/iter",
            "extra": "iterations: 2254516\ncpu: 0.31009772474446845 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.35492088313439135,
            "unit": "us/iter",
            "extra": "iterations: 1973018\ncpu: 0.3549073439776018 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3110421845608213,
            "unit": "us/iter",
            "extra": "iterations: 2251345\ncpu: 0.3110233358281384 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.70282958599979,
            "unit": "us/iter",
            "extra": "iterations: 16812\ncpu: 41.699262610040414 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 64.02513386831298,
            "unit": "us/iter",
            "extra": "iterations: 10966\ncpu: 64.01981725332845 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.25529789942757,
            "unit": "us/iter",
            "extra": "iterations: 21994\ncpu: 32.253670864781284 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.861165358702486,
            "unit": "us/iter",
            "extra": "iterations: 10831\ncpu: 63.856304773335786 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.911764540182013,
            "unit": "us/iter",
            "extra": "iterations: 35144\ncpu: 19.911065985659036 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.253640958960508,
            "unit": "us/iter",
            "extra": "iterations: 49220\ncpu: 14.252641466883377 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 19.928867873508366,
            "unit": "us/iter",
            "extra": "iterations: 35133\ncpu: 19.92756135257449 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.29099422177899,
            "unit": "us/iter",
            "extra": "iterations: 95185\ncpu: 7.290625203550991 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.78550398090252,
            "unit": "us/iter",
            "extra": "iterations: 11304\ncpu: 61.77835129157814 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.72445735995503,
            "unit": "us/iter",
            "extra": "iterations: 8818\ncpu: 78.71991075073703 us\nthreads: 1"
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
        "date": 1752241160051,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35668551550168354,
            "unit": "us/iter",
            "extra": "iterations: 1907598\ncpu: 0.3566652811546248 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.31537097743114245,
            "unit": "us/iter",
            "extra": "iterations: 2222305\ncpu: 0.31533264245906856 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.3507923087041155,
            "unit": "us/iter",
            "extra": "iterations: 2007022\ncpu: 0.35077565367992963 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3114515168918481,
            "unit": "us/iter",
            "extra": "iterations: 2242678\ncpu: 0.31139794879157845 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.340145616730375,
            "unit": "us/iter",
            "extra": "iterations: 16928\ncpu: 41.336839024102076 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 64.2835744036664,
            "unit": "us/iter",
            "extra": "iterations: 10900\ncpu: 64.27462990825687 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.813054028852207,
            "unit": "us/iter",
            "extra": "iterations: 22525\ncpu: 31.811573940066598 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 64.25489092407275,
            "unit": "us/iter",
            "extra": "iterations: 10919\ncpu: 64.24489376316507 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.014752362727265,
            "unit": "us/iter",
            "extra": "iterations: 35023\ncpu: 20.013889843816955 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.117940148000589,
            "unit": "us/iter",
            "extra": "iterations: 49188\ncpu: 14.115215398064587 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.150423586317416,
            "unit": "us/iter",
            "extra": "iterations: 34732\ncpu: 20.148756852470363 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.4181927201421995,
            "unit": "us/iter",
            "extra": "iterations: 94370\ncpu: 7.417854508848144 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.810406305049014,
            "unit": "us/iter",
            "extra": "iterations: 11356\ncpu: 61.80166194082417 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 79.62362818640467,
            "unit": "us/iter",
            "extra": "iterations: 8905\ncpu: 79.58138562605279 us\nthreads: 1"
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
        "date": 1758812356498,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35887145405298393,
            "unit": "us/iter",
            "extra": "iterations: 1949303\ncpu: 0.3588329248967451 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.31003656428222914,
            "unit": "us/iter",
            "extra": "iterations: 2261606\ncpu: 0.3100093884611201 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.34603835663630234,
            "unit": "us/iter",
            "extra": "iterations: 2020198\ncpu: 0.3459848613848742 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3095093418389272,
            "unit": "us/iter",
            "extra": "iterations: 2262349\ncpu: 0.30947992993123513 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.374742486982534,
            "unit": "us/iter",
            "extra": "iterations: 16904\ncpu: 41.370155702792275 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 65.61464627583216,
            "unit": "us/iter",
            "extra": "iterations: 10593\ncpu: 65.60887255734924 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.355711643685453,
            "unit": "us/iter",
            "extra": "iterations: 22115\ncpu: 31.351891702464393 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 65.63859540768203,
            "unit": "us/iter",
            "extra": "iterations: 10670\ncpu: 65.63039831302711 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.027126812945333,
            "unit": "us/iter",
            "extra": "iterations: 34957\ncpu: 20.02493366135539 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.083125461924096,
            "unit": "us/iter",
            "extra": "iterations: 49521\ncpu: 14.082393731952108 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.099766677226036,
            "unit": "us/iter",
            "extra": "iterations: 34733\ncpu: 20.09839581953761 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.3471873820026135,
            "unit": "us/iter",
            "extra": "iterations: 95340\ncpu: 7.346609911894279 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.641921277169175,
            "unit": "us/iter",
            "extra": "iterations: 11369\ncpu: 61.637129474887885 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 80.1783208175467,
            "unit": "us/iter",
            "extra": "iterations: 8709\ncpu: 80.16973337926294 us\nthreads: 1"
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
        "date": 1759938069633,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3566305103554741,
            "unit": "us/iter",
            "extra": "iterations: 1957224\ncpu: 0.3566106086988511 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3100177927250307,
            "unit": "us/iter",
            "extra": "iterations: 2259800\ncpu: 0.3099842132047085 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.34529742959399407,
            "unit": "us/iter",
            "extra": "iterations: 2029835\ncpu: 0.3452804065355066 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3097977354291159,
            "unit": "us/iter",
            "extra": "iterations: 2261444\ncpu: 0.30975608858764564 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.35654339844562,
            "unit": "us/iter",
            "extra": "iterations: 16890\ncpu: 41.35426980461808 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 64.1154405741407,
            "unit": "us/iter",
            "extra": "iterations: 10938\ncpu: 64.10958584750414 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.114369525628064,
            "unit": "us/iter",
            "extra": "iterations: 21966\ncpu: 32.11021947555313 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 64.18030504756865,
            "unit": "us/iter",
            "extra": "iterations: 10936\ncpu: 64.1716674286759 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.875891177980925,
            "unit": "us/iter",
            "extra": "iterations: 35094\ncpu: 19.873662420926653 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.164456119974552,
            "unit": "us/iter",
            "extra": "iterations: 49077\ncpu: 14.163084785133574 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 19.917448265082438,
            "unit": "us/iter",
            "extra": "iterations: 35189\ncpu: 19.91547125522183 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.331268502976948,
            "unit": "us/iter",
            "extra": "iterations: 95917\ncpu: 7.330684060177016 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.82642984077748,
            "unit": "us/iter",
            "extra": "iterations: 11367\ncpu: 61.81860147796244 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.70584830452277,
            "unit": "us/iter",
            "extra": "iterations: 8906\ncpu: 78.69137334381321 us\nthreads: 1"
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
        "date": 1760035273066,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3575352552271657,
            "unit": "us/iter",
            "extra": "iterations: 1958589\ncpu: 0.3575117163427345 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.31601981320215394,
            "unit": "us/iter",
            "extra": "iterations: 2216300\ncpu: 0.31599399404412765 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.3452309886039713,
            "unit": "us/iter",
            "extra": "iterations: 2026403\ncpu: 0.34521487088205055 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3123989689001244,
            "unit": "us/iter",
            "extra": "iterations: 2241102\ncpu: 0.3123784602396499 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.386367985830454,
            "unit": "us/iter",
            "extra": "iterations: 16930\ncpu: 41.382896810395735 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.587253335761886,
            "unit": "us/iter",
            "extra": "iterations: 11017\ncpu: 63.5838313515476 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.260525540860726,
            "unit": "us/iter",
            "extra": "iterations: 22141\ncpu: 32.259298721828266 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.71190596892664,
            "unit": "us/iter",
            "extra": "iterations: 11007\ncpu: 63.707641682565715 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.961791419947044,
            "unit": "us/iter",
            "extra": "iterations: 35128\ncpu: 19.960587394670945 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.179854452648392,
            "unit": "us/iter",
            "extra": "iterations: 49173\ncpu: 14.178706261566301 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 19.96249970054165,
            "unit": "us/iter",
            "extra": "iterations: 35063\ncpu: 19.961642243960885 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.2861304623381935,
            "unit": "us/iter",
            "extra": "iterations: 96250\ncpu: 7.28561594805195 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.65599700545855,
            "unit": "us/iter",
            "extra": "iterations: 11354\ncpu: 61.65340611238323 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.39262536379049,
            "unit": "us/iter",
            "extra": "iterations: 8934\ncpu: 78.38864719050821 us\nthreads: 1"
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
        "date": 1760855386839,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3562932016380275,
            "unit": "us/iter",
            "extra": "iterations: 1961193\ncpu: 0.35627785383692473 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3077870706669635,
            "unit": "us/iter",
            "extra": "iterations: 2272012\ncpu: 0.3077635888366787 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.34542592841362973,
            "unit": "us/iter",
            "extra": "iterations: 2024798\ncpu: 0.3454098265604765 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3081611737236085,
            "unit": "us/iter",
            "extra": "iterations: 2276345\ncpu: 0.3081390812904022 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.39901767662388,
            "unit": "us/iter",
            "extra": "iterations: 16915\ncpu: 41.39644528524978 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.897104719231315,
            "unit": "us/iter",
            "extra": "iterations: 10934\ncpu: 63.89151289555514 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.307382009071695,
            "unit": "us/iter",
            "extra": "iterations: 20955\ncpu: 31.305695585779073 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.825185736463354,
            "unit": "us/iter",
            "extra": "iterations: 10951\ncpu: 63.821640580768864 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.943587955446066,
            "unit": "us/iter",
            "extra": "iterations: 35103\ncpu: 19.942657037860013 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.220608292413493,
            "unit": "us/iter",
            "extra": "iterations: 49491\ncpu: 14.21989230365118 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 19.946534260233214,
            "unit": "us/iter",
            "extra": "iterations: 35099\ncpu: 19.94591182084957 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.333349925438914,
            "unit": "us/iter",
            "extra": "iterations: 95226\ncpu: 7.3331511561968306 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.676667341357316,
            "unit": "us/iter",
            "extra": "iterations: 11363\ncpu: 61.67247637067669 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 79.21792004976369,
            "unit": "us/iter",
            "extra": "iterations: 8843\ncpu: 79.21758882732115 us\nthreads: 1"
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
        "date": 1761580212092,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.32581928930704224,
            "unit": "us/iter",
            "extra": "iterations: 2149170\ncpu: 0.32579383250278016 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.2903984711762088,
            "unit": "us/iter",
            "extra": "iterations: 2409434\ncpu: 0.29037617880381855 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.31593524622519725,
            "unit": "us/iter",
            "extra": "iterations: 2221523\ncpu: 0.31591037770034347 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.28952625802865,
            "unit": "us/iter",
            "extra": "iterations: 2410120\ncpu: 0.28949348870595665 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 40.233826655909,
            "unit": "us/iter",
            "extra": "iterations: 17347\ncpu: 40.230180838185255 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 60.51136294570082,
            "unit": "us/iter",
            "extra": "iterations: 11583\ncpu: 60.50655633255631 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 29.88893766821503,
            "unit": "us/iter",
            "extra": "iterations: 23407\ncpu: 29.887516042209626 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 60.63906704782437,
            "unit": "us/iter",
            "extra": "iterations: 11544\ncpu: 60.631001645876694 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 17.208472253035765,
            "unit": "us/iter",
            "extra": "iterations: 40563\ncpu: 17.20732376796588 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.730174529200758,
            "unit": "us/iter",
            "extra": "iterations: 47207\ncpu: 14.728783824432812 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 17.463545074508737,
            "unit": "us/iter",
            "extra": "iterations: 40666\ncpu: 17.462616460925563 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 6.965281481555045,
            "unit": "us/iter",
            "extra": "iterations: 100678\ncpu: 6.964981833171102 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 57.8221784622826,
            "unit": "us/iter",
            "extra": "iterations: 12109\ncpu: 57.819854983896235 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 76.46484343653545,
            "unit": "us/iter",
            "extra": "iterations: 9172\ncpu: 76.45586317051897 us\nthreads: 1"
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
        "date": 1761668676728,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3569328459833604,
            "unit": "us/iter",
            "extra": "iterations: 1956294\ncpu: 0.3569113287675573 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3118985263263886,
            "unit": "us/iter",
            "extra": "iterations: 2243577\ncpu: 0.311873561727545 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.34491527049788584,
            "unit": "us/iter",
            "extra": "iterations: 2022141\ncpu: 0.3448805666864972 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3123654017868306,
            "unit": "us/iter",
            "extra": "iterations: 2244324\ncpu: 0.31233752791486447 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.347437998471825,
            "unit": "us/iter",
            "extra": "iterations: 16927\ncpu: 41.345166656820446 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 64.20408197469045,
            "unit": "us/iter",
            "extra": "iterations: 10918\ncpu: 64.19859937717537 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.20500983503981,
            "unit": "us/iter",
            "extra": "iterations: 22369\ncpu: 32.20344552729225 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 64.11415674731504,
            "unit": "us/iter",
            "extra": "iterations: 10871\ncpu: 64.11256600128787 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.142098316062093,
            "unit": "us/iter",
            "extra": "iterations: 34918\ncpu: 20.14116856635546 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.226099598036821,
            "unit": "us/iter",
            "extra": "iterations: 49258\ncpu: 14.225219984571064 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.137037730433875,
            "unit": "us/iter",
            "extra": "iterations: 34879\ncpu: 20.13591937842256 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.261749602070862,
            "unit": "us/iter",
            "extra": "iterations: 96123\ncpu: 7.261424133662087 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.587265652483474,
            "unit": "us/iter",
            "extra": "iterations: 11372\ncpu: 61.58159628913114 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.76301734036947,
            "unit": "us/iter",
            "extra": "iterations: 8881\ncpu: 78.75809683594188 us\nthreads: 1"
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
          "id": "d2a3b9b38f17b1fe41c3923e0a10d956adfdeffc",
          "message": "chore: Enhance protocol validation and link to specs (#60)\n\nThis commit strengthens security checks in PVE and DKG protocols and adds traceability by linking KEM/DEM functions to the `basic-primitives-spec`.\n\n* PVE: Adds explicit point-on-curve checks in `pve_ac.cpp` and `pve_batch.cpp` to prevent invalid-curve attacks.\n* DKG: Adds a public key reconstruction check in `ec_dkg.cpp` to verify the new shares match the original group key.\n* PVE Batch: Validates the DRBG seed length and refactors RO hashing to use the generic `bitlen(SEC_P_COM)` instead of a hardcoded `bitlen128()`.\n* PKI: Adds `@specs` tags to `seal`, `open`, `encapsulate`, and `decapsulate` in `base_pki.h` and renames `pkR` to `pub_key` for clarity.",
          "timestamp": "2025-11-04T09:19:52-08:00",
          "tree_id": "70c88dc37f8ab1f777ee770ea4b6932f08052d64",
          "url": "https://github.com/coinbase/cb-mpc/commit/d2a3b9b38f17b1fe41c3923e0a10d956adfdeffc"
        },
        "date": 1762279052878,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35685729004642586,
            "unit": "us/iter",
            "extra": "iterations: 1955694\ncpu: 0.356831804975625 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.30810472015164053,
            "unit": "us/iter",
            "extra": "iterations: 2271855\ncpu: 0.3080972416813572 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.34574396360204507,
            "unit": "us/iter",
            "extra": "iterations: 2018795\ncpu: 0.3457175706300045 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3080159354091805,
            "unit": "us/iter",
            "extra": "iterations: 2270290\ncpu: 0.3080015795338923 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.362849331274596,
            "unit": "us/iter",
            "extra": "iterations: 16898\ncpu: 41.35774576873002 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.33409141048935,
            "unit": "us/iter",
            "extra": "iterations: 11060\ncpu: 63.33312432188067 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.647458612867496,
            "unit": "us/iter",
            "extra": "iterations: 21613\ncpu: 31.645482024707334 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.29617775374425,
            "unit": "us/iter",
            "extra": "iterations: 11094\ncpu: 63.29284270777004 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.033414829830637,
            "unit": "us/iter",
            "extra": "iterations: 35024\ncpu: 20.03217176793059 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.265113151779042,
            "unit": "us/iter",
            "extra": "iterations: 49058\ncpu: 14.263909882180277 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.003512235944687,
            "unit": "us/iter",
            "extra": "iterations: 34652\ncpu: 20.00160285120628 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.340640933986416,
            "unit": "us/iter",
            "extra": "iterations: 95676\ncpu: 7.339181497972333 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.705016286637054,
            "unit": "us/iter",
            "extra": "iterations: 11359\ncpu: 61.70402412184164 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.08221673772667,
            "unit": "us/iter",
            "extra": "iterations: 8914\ncpu: 78.07719564729638 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}