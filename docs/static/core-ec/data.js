window.BENCHMARK_DATA = {
  "lastUpdate": 1746632562265,
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
      }
    ]
  }
}