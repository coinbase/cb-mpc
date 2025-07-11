window.BENCHMARK_DATA = {
  "lastUpdate": 1752241160866,
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
      }
    ]
  }
}