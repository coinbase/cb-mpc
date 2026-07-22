window.BENCHMARK_DATA = {
  "lastUpdate": 1784754269226,
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
          "id": "33f98c9658d4b48463eba7b3e8dd06ffe5a8f8ee",
          "message": "feat: Add constant-time curve operations and enhancements (#68)\n\nThis commit introduces constant-time elliptic curve operations and related improvements to the cryptographic primitives:\n\n- Add ct_add_support enum and consttime_point_add_scope for managing constant-time point addition capabilities across different curve backends\n- Implement weighted_sum() for secure multi-scalar multiplication\n- Replace extended_ec_mul_add_ct with ecc_point_t::weighted_sum for consistency\n- Add ct_add_support() implementation for Ed25519 (full support)\n- Enhance Paillier with rand_N_star() for sampling Z_N^*\n- Refactor ZK utilities: rename get_13_bits to get_log_alpha_bits and introduce uint_mem_array_t template for safer bit operations\n- Update curve_msg_to_bn usage in ECDSA 2-party implementation",
          "timestamp": "2025-12-04T09:46:31-08:00",
          "tree_id": "b2e208717ed56ef7a5e1f1788c6a10e92e506fe5",
          "url": "https://github.com/coinbase/cb-mpc/commit/33f98c9658d4b48463eba7b3e8dd06ffe5a8f8ee"
        },
        "date": 1764872741975,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35573201678502286,
            "unit": "us/iter",
            "extra": "iterations: 1961037\ncpu: 0.3557080881186842 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3091860371438344,
            "unit": "us/iter",
            "extra": "iterations: 2269020\ncpu: 0.30917792086451434 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.3514262956444134,
            "unit": "us/iter",
            "extra": "iterations: 1997616\ncpu: 0.35140508035578427 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3129270819533126,
            "unit": "us/iter",
            "extra": "iterations: 2241777\ncpu: 0.3129232242992947 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.588807102076984,
            "unit": "us/iter",
            "extra": "iterations: 16812\ncpu: 41.585964965500835 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.720031831458826,
            "unit": "us/iter",
            "extra": "iterations: 10964\ncpu: 63.7178587194454 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.174532085809666,
            "unit": "us/iter",
            "extra": "iterations: 21910\ncpu: 32.17158662711091 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 64.94273910666148,
            "unit": "us/iter",
            "extra": "iterations: 10970\ncpu: 64.9376216955333 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.942767172291962,
            "unit": "us/iter",
            "extra": "iterations: 34940\ncpu: 19.941073325701225 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.218528151171581,
            "unit": "us/iter",
            "extra": "iterations: 48950\ncpu: 14.217861736465764 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 19.96547981667241,
            "unit": "us/iter",
            "extra": "iterations: 35128\ncpu: 19.963796601002073 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.401991032568411,
            "unit": "us/iter",
            "extra": "iterations: 95122\ncpu: 7.401285033956402 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.893082943614075,
            "unit": "us/iter",
            "extra": "iterations: 11333\ncpu: 61.890073502161705 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.64838869573609,
            "unit": "us/iter",
            "extra": "iterations: 8917\ncpu: 78.64538163059319 us\nthreads: 1"
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
          "id": "6493b1df0ce2622bde41eeebead94a91926c964e",
          "message": "fix: Correct benchmark function references, entropy seeds (#72)\n\n- Fix BENCHMARK() macros to reference correct functions (BM_HMAC_SHA256, BM_AES_GCM_128, BM_AES_GCM_256) instead of wrong ones\n- Fix typo: BM_AEC_GCM -> BM_AES_GCM\n- Add benchmark::DoNotOptimize() to prevent compiler from optimizing out hash/encryption results\n- Use proper DRBG seeds with sufficient entropy (16 bytes) instead of short \"test\" string that doesn't meet SEC_P_COM requirement\n- Adjust benchmark input ranges for more meaningful measurements",
          "timestamp": "2025-12-11T17:35:43-08:00",
          "tree_id": "6e499c524dbf2e11241e3493897fa826b8e77a71",
          "url": "https://github.com/coinbase/cb-mpc/commit/6493b1df0ce2622bde41eeebead94a91926c964e"
        },
        "date": 1765505729706,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35675881878121934,
            "unit": "us/iter",
            "extra": "iterations: 1959908\ncpu: 0.35674971121093435 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3101238576485969,
            "unit": "us/iter",
            "extra": "iterations: 2258609\ncpu: 0.3100959550767751 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.3502032122964889,
            "unit": "us/iter",
            "extra": "iterations: 2000189\ncpu: 0.35016828659691646 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.31194669821138493,
            "unit": "us/iter",
            "extra": "iterations: 2248236\ncpu: 0.3119353146199952 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.48393628611302,
            "unit": "us/iter",
            "extra": "iterations: 16888\ncpu: 41.48195783988629 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 64.09673300348346,
            "unit": "us/iter",
            "extra": "iterations: 10914\ncpu: 64.09182957669051 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.749011653080693,
            "unit": "us/iter",
            "extra": "iterations: 21711\ncpu: 31.74696904794805 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 64.07228515267059,
            "unit": "us/iter",
            "extra": "iterations: 10938\ncpu: 64.06924611446334 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.999701932713194,
            "unit": "us/iter",
            "extra": "iterations: 34925\ncpu: 19.99783055118112 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.269797094055788,
            "unit": "us/iter",
            "extra": "iterations: 49003\ncpu: 14.26904207905637 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.03239659611182,
            "unit": "us/iter",
            "extra": "iterations: 34960\ncpu: 20.031447139588092 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.284110295338098,
            "unit": "us/iter",
            "extra": "iterations: 96296\ncpu: 7.283708316025588 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.762509575496075,
            "unit": "us/iter",
            "extra": "iterations: 11331\ncpu: 61.76061936280999 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.60748082957635,
            "unit": "us/iter",
            "extra": "iterations: 8920\ncpu: 78.60463699551563 us\nthreads: 1"
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
          "id": "1bc6a7de5b9004d246325a30ab9a63344ffb351e",
          "message": "chore: remove update function in TDH2 (#73)",
          "timestamp": "2025-12-12T07:59:40-08:00",
          "tree_id": "d638d2cbcb63c4b843602a8601e465a937103925",
          "url": "https://github.com/coinbase/cb-mpc/commit/1bc6a7de5b9004d246325a30ab9a63344ffb351e"
        },
        "date": 1765557539029,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3569797441130054,
            "unit": "us/iter",
            "extra": "iterations: 1953753\ncpu: 0.3569545285407111 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3097842653670513,
            "unit": "us/iter",
            "extra": "iterations: 2255623\ncpu: 0.3097637960776247 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.3497168065087595,
            "unit": "us/iter",
            "extra": "iterations: 1995346\ncpu: 0.34970100373569285 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.31412820124056845,
            "unit": "us/iter",
            "extra": "iterations: 2229292\ncpu: 0.31412715471997393 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.57333690441968,
            "unit": "us/iter",
            "extra": "iterations: 16895\ncpu: 41.56883681562594 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.86312020413355,
            "unit": "us/iter",
            "extra": "iterations: 10973\ncpu: 63.86000829308303 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.771244315839407,
            "unit": "us/iter",
            "extra": "iterations: 21771\ncpu: 31.769407468650957 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.93096160526206,
            "unit": "us/iter",
            "extra": "iterations: 10939\ncpu: 63.92667346192514 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.10395085839351,
            "unit": "us/iter",
            "extra": "iterations: 34716\ncpu: 20.10310263279181 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.250012642634243,
            "unit": "us/iter",
            "extra": "iterations: 49515\ncpu: 14.249529011410683 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.090167644950256,
            "unit": "us/iter",
            "extra": "iterations: 34925\ncpu: 20.089461360057253 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.272688199013994,
            "unit": "us/iter",
            "extra": "iterations: 96475\ncpu: 7.272175506607926 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.88535374569931,
            "unit": "us/iter",
            "extra": "iterations: 11333\ncpu: 61.8842942733609 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.49658409957053,
            "unit": "us/iter",
            "extra": "iterations: 8918\ncpu: 78.49338596097773 us\nthreads: 1"
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
          "id": "7bca8268c9338298ea949fef1258a3d6b066f47d",
          "message": "bench: Add curve check benchmark and improve benchmark practices (#74)",
          "timestamp": "2025-12-12T09:56:25-08:00",
          "tree_id": "fbacef40e1c997af50a3aaceb751fe976a8af082",
          "url": "https://github.com/coinbase/cb-mpc/commit/7bca8268c9338298ea949fef1258a3d6b066f47d"
        },
        "date": 1765564578998,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3566039459200489,
            "unit": "us/iter",
            "extra": "iterations: 1956806\ncpu: 0.3565974358214355 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3094940555744838,
            "unit": "us/iter",
            "extra": "iterations: 2259596\ncpu: 0.3094778721505968 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.34963864018168866,
            "unit": "us/iter",
            "extra": "iterations: 1994090\ncpu: 0.3496350044381147 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.311217287377666,
            "unit": "us/iter",
            "extra": "iterations: 2248658\ncpu: 0.3112007188287412 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.53896825302382,
            "unit": "us/iter",
            "extra": "iterations: 16852\ncpu: 41.537928554474234 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.350901848832514,
            "unit": "us/iter",
            "extra": "iterations: 11034\ncpu: 63.34761899583105 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 30.726609883988534,
            "unit": "us/iter",
            "extra": "iterations: 21550\ncpu: 30.72511684454755 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.635511218083856,
            "unit": "us/iter",
            "extra": "iterations: 11009\ncpu: 63.630211645017766 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.071333323795983,
            "unit": "us/iter",
            "extra": "iterations: 34963\ncpu: 20.07111223293195 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.317301577262006,
            "unit": "us/iter",
            "extra": "iterations: 49009\ncpu: 14.316747066865275 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.076451672435837,
            "unit": "us/iter",
            "extra": "iterations: 34680\ncpu: 20.075306632064564 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.306458523377259,
            "unit": "us/iter",
            "extra": "iterations: 96030\ncpu: 7.306361668228673 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.88122374468496,
            "unit": "us/iter",
            "extra": "iterations: 11312\ncpu: 61.876137641442796 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.50184991587989,
            "unit": "us/iter",
            "extra": "iterations: 8915\ncpu: 78.4999568143578 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 1.9942398002496924,
            "unit": "us/iter",
            "extra": "iterations: 349837\ncpu: 1.994105906465011 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 63.674905581359624,
            "unit": "us/iter",
            "extra": "iterations: 10983\ncpu: 63.67312674132749 us\nthreads: 1"
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
          "id": "44d3ccf04218bb59833b7977431596a7e4ee490a",
          "message": "refactor: extract FFI memory adapters into separate cbmpc/ffi module (#75)\n\nMove C memory allocation/deallocation utilities (cmem_t, cmems_t) from core/buf and crypto/base_pki into a dedicated ffi module. This separates FFI-specific concerns from core library types and provides cleaner abstraction for Go bindings.\n\nChanges:\n- Create src/cbmpc/ffi with cmem_adapter.cpp/h for C memory management\n- Move ffi_kem_ek_t and ffi_kem_dk_t types to ffi/pki.h\n- Update all CGO bindings to use ffi::view() and ffi::copy_to_cmem()\n- Remove to_cmem/from_cmem methods from buf_t and mem_t\n- Remove mems_t class and related FFI conversions from core",
          "timestamp": "2025-12-15T08:38:15-08:00",
          "tree_id": "905b8807be03707b75fc8497009f55950b9087d1",
          "url": "https://github.com/coinbase/cb-mpc/commit/44d3ccf04218bb59833b7977431596a7e4ee490a"
        },
        "date": 1765819031746,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3564329592288423,
            "unit": "us/iter",
            "extra": "iterations: 1962910\ncpu: 0.35640916700205316 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.31127608905958143,
            "unit": "us/iter",
            "extra": "iterations: 2241016\ncpu: 0.31125994281165326 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.35574233156341567,
            "unit": "us/iter",
            "extra": "iterations: 1963112\ncpu: 0.3557133367836375 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3136816437952647,
            "unit": "us/iter",
            "extra": "iterations: 2228331\ncpu: 0.31367106592333016 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.440031433830335,
            "unit": "us/iter",
            "extra": "iterations: 16829\ncpu: 41.43944173747695 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 63.43633846153439,
            "unit": "us/iter",
            "extra": "iterations: 11050\ncpu: 63.43533520361996 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.26773621179818,
            "unit": "us/iter",
            "extra": "iterations: 21794\ncpu: 32.26687271726165 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 63.29319563643098,
            "unit": "us/iter",
            "extra": "iterations: 11046\ncpu: 63.290752489589046 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.96719936268065,
            "unit": "us/iter",
            "extra": "iterations: 35147\ncpu: 19.96552257660682 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.216490793689609,
            "unit": "us/iter",
            "extra": "iterations: 49314\ncpu: 14.216107109542925 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 19.996286222117945,
            "unit": "us/iter",
            "extra": "iterations: 34882\ncpu: 19.995213605871232 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.452137037670816,
            "unit": "us/iter",
            "extra": "iterations: 93602\ncpu: 7.451748926304989 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 61.72097106451949,
            "unit": "us/iter",
            "extra": "iterations: 11301\ncpu: 61.717352535173895 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 77.85833218186714,
            "unit": "us/iter",
            "extra": "iterations: 8974\ncpu: 77.85079061733904 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 1.9985509165518949,
            "unit": "us/iter",
            "extra": "iterations: 351153\ncpu: 1.9984226505255518 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 63.72867849325391,
            "unit": "us/iter",
            "extra": "iterations: 10964\ncpu: 63.72539137176206 us\nthreads: 1"
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
          "id": "f36b009747054b1dba30b9affde4409f76976af0",
          "message": "fix: security hardening for ZK proofs and protocol input validation (#84)",
          "timestamp": "2026-01-07T10:55:24-08:00",
          "tree_id": "22191982796070d833dd23436687c56cc6289f7b",
          "url": "https://github.com/coinbase/cb-mpc/commit/f36b009747054b1dba30b9affde4409f76976af0"
        },
        "date": 1767814584195,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35695718112199976,
            "unit": "us/iter",
            "extra": "iterations: 1959276\ncpu: 0.35693611823959465 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3114864034857799,
            "unit": "us/iter",
            "extra": "iterations: 2245833\ncpu: 0.31145414997464177 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 0.37092056169040966,
            "unit": "us/iter",
            "extra": "iterations: 1888371\ncpu: 0.37089876777391734 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3135612716745443,
            "unit": "us/iter",
            "extra": "iterations: 2234597\ncpu: 0.313534980580391 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.62781599096788,
            "unit": "us/iter",
            "extra": "iterations: 16847\ncpu: 41.62396765002669 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 64.12832985921703,
            "unit": "us/iter",
            "extra": "iterations: 10938\ncpu: 64.12153391844951 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.334561407547195,
            "unit": "us/iter",
            "extra": "iterations: 21740\ncpu: 31.334076356945694 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 64.13775568338404,
            "unit": "us/iter",
            "extra": "iterations: 10953\ncpu: 64.13079539852095 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.9880953360244,
            "unit": "us/iter",
            "extra": "iterations: 35013\ncpu: 19.98579641847314 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.41200168717922,
            "unit": "us/iter",
            "extra": "iterations: 48602\ncpu: 14.410527550306568 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 19.992473706734927,
            "unit": "us/iter",
            "extra": "iterations: 35066\ncpu: 19.989392545485654 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.400112922335478,
            "unit": "us/iter",
            "extra": "iterations: 94534\ncpu: 7.398984111536599 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 62.19932608696753,
            "unit": "us/iter",
            "extra": "iterations: 11224\ncpu: 62.19260522095508 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 79.37424694707221,
            "unit": "us/iter",
            "extra": "iterations: 8844\ncpu: 79.36159848484849 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 2.1918891894701944,
            "unit": "us/iter",
            "extra": "iterations: 317533\ncpu: 2.191620407957597 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 64.39032706767212,
            "unit": "us/iter",
            "extra": "iterations: 10906\ncpu: 64.38123565010082 us\nthreads: 1"
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
          "id": "82e269131b125cec6164c70a1a68fe67b7160816",
          "message": "fix: constant-time hardening and input validation across crypto primitives (#88)\n\nSecurity improvements for side-channel resistance:\n- Fix Edwards curve identity detection to use (X=0 && Y==Z), avoiding confusion with torsion points like (0,-1)\n- Upgrade secp256k1 point addition to fully constant-time using affine conversion + cmov pattern, removing conditional CT fallback\n- Add constant-time conditional copy (cnd_copy_point) for secp256k1 and Ed25519\n- Fix bn_cmp_ct to properly handle comparison of negative numbers\n- Add constant_time_select_bytes/ct_select_buf utilities\n- Enforce CT-capable curves for ElGamal operations; reduce scalars mod q\n\nInput validation hardening:\n- Add range checks for Paillier encrypt/decrypt inputs\n- Change paillier_t::rand_N_star to return error on gcd(r,N)!=1 by default\n- Convert commitment::verify asserts to proper error returns\n- Add ro_t::get_buf_checked with output length validation\n- Validate share indices and threshold in secret sharing\n- Add input validation across ZK proofs (Paillier, ElGamal, Pedersen)\n- Add protocol-level validation in ECDSA 2P/MP, OT, PVE, Schnorr, DKG\n\nOther fixes:\n- Fix memory leak and uninitialized cmems_t in Go FFI transport\n- Add verify_out_of_band for MPC session transport verification\n- Remove unused pbkdf2, ct_add_support_e::Conditional, ZK batch functions",
          "timestamp": "2026-01-20T07:34:06-08:00",
          "tree_id": "a5f10c451a147e4d1d376ad62b26dd7c32364835",
          "url": "https://github.com/coinbase/cb-mpc/commit/82e269131b125cec6164c70a1a68fe67b7160816"
        },
        "date": 1768925442283,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35728428934961154,
            "unit": "us/iter",
            "extra": "iterations: 1952268\ncpu: 0.3572620475262618 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.35450004133901675,
            "unit": "us/iter",
            "extra": "iterations: 1886836\ncpu: 0.3545001171272967 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 2.259332645633303,
            "unit": "us/iter",
            "extra": "iterations: 308274\ncpu: 2.259185974165841 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.35670201461088846,
            "unit": "us/iter",
            "extra": "iterations: 1960180\ncpu: 0.3566800538726034 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.48047397649034,
            "unit": "us/iter",
            "extra": "iterations: 16927\ncpu: 41.47799474212796 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 65.1281532670434,
            "unit": "us/iter",
            "extra": "iterations: 10759\ncpu: 65.12330876475514 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.529111011853114,
            "unit": "us/iter",
            "extra": "iterations: 21268\ncpu: 31.527375070528503 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 65.49631307188635,
            "unit": "us/iter",
            "extra": "iterations: 10710\ncpu: 65.49097114845945 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.98262845410472,
            "unit": "us/iter",
            "extra": "iterations: 35067\ncpu: 19.980537913137717 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.20266047085253,
            "unit": "us/iter",
            "extra": "iterations: 49442\ncpu: 14.201353343311327 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.043642651460686,
            "unit": "us/iter",
            "extra": "iterations: 35075\ncpu: 20.04030782608698 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.182342820155564,
            "unit": "us/iter",
            "extra": "iterations: 97328\ncpu: 7.181733016192672 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 63.600258023451495,
            "unit": "us/iter",
            "extra": "iterations: 10999\ncpu: 63.59859650877347 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 78.78812580172387,
            "unit": "us/iter",
            "extra": "iterations: 8887\ncpu: 78.78229717564989 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 1.9998494234784987,
            "unit": "us/iter",
            "extra": "iterations: 349510\ncpu: 1.9996705788103377 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 64.47156697990819,
            "unit": "us/iter",
            "extra": "iterations: 10854\ncpu: 64.46057259996323 us\nthreads: 1"
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
          "id": "0460152b452fd76fab506c945674cff07ea7b162",
          "message": "fix: harden additive share reconstruction (#114)\n\nTrack access-structure satisfaction separately for child nodes so OR and threshold reconstruction select only satisfied subtrees when producing additive private and public shares.\n\nAdd ECDKG coverage for redundant and partially unsatisfied composite children.\n\nUse full-width Fischlin hash values to avoid truncating accepted parameter ranges, and correct the Shamir benchmark argument ordering.",
          "timestamp": "2026-05-15T07:16:43-07:00",
          "tree_id": "2423fde4db9e98caffc745a5016877cf2e3163fb",
          "url": "https://github.com/coinbase/cb-mpc/commit/0460152b452fd76fab506c945674cff07ea7b162"
        },
        "date": 1778856939891,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.404044253910015,
            "unit": "us/iter",
            "extra": "iterations: 1731621\ncpu: 0.4040121290975335 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.39557723062112704,
            "unit": "us/iter",
            "extra": "iterations: 1769227\ncpu: 0.3955428681565451 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 2.759748341214173,
            "unit": "us/iter",
            "extra": "iterations: 262240\ncpu: 2.758842930140329 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.39726779538974755,
            "unit": "us/iter",
            "extra": "iterations: 1749765\ncpu: 0.397242893188514 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 45.802162822503306,
            "unit": "us/iter",
            "extra": "iterations: 15348\ncpu: 45.796014594735446 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 77.91498164629355,
            "unit": "us/iter",
            "extra": "iterations: 8990\ncpu: 77.90603092324811 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 35.1041735173777,
            "unit": "us/iter",
            "extra": "iterations: 19560\ncpu: 35.099539519427424 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 77.71237132638572,
            "unit": "us/iter",
            "extra": "iterations: 8949\ncpu: 77.70364498826684 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.683683615657568,
            "unit": "us/iter",
            "extra": "iterations: 33886\ncpu: 20.68172637667472 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 15.691826296958592,
            "unit": "us/iter",
            "extra": "iterations: 44720\ncpu: 15.690560979427536 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.679246925457264,
            "unit": "us/iter",
            "extra": "iterations: 33257\ncpu: 20.67697621553359 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 8.126424897822119,
            "unit": "us/iter",
            "extra": "iterations: 86369\ncpu: 8.125338697912436 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 68.93939736400891,
            "unit": "us/iter",
            "extra": "iterations: 10167\ncpu: 68.9326167994493 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 93.97673925885591,
            "unit": "us/iter",
            "extra": "iterations: 7448\ncpu: 93.9660635069818 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 2.4114273984783527,
            "unit": "us/iter",
            "extra": "iterations: 290111\ncpu: 2.4112556435295445 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 78.20790296918807,
            "unit": "us/iter",
            "extra": "iterations: 8925\ncpu: 78.20354218487398 us\nthreads: 1"
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
          "id": "a8ee24c6d48f11d2f61e005e3ed0c5056d42bedc",
          "message": "feat: add ML-KEM-768 and extend bn256_t/DRBG/RO primitives (#115)\n\n- Add mlkem768_pub_key_t and mlkem768_prv_key_t backed by OpenSSL ML-KEM-768 for post-quantum key encapsulation, with unit tests\n- Extend bn256_t with comparison operators (<, >, <=, >=), static modular helpers (add_mod, sub_mod, neg_mod), a generic reduce(mod) overload, and SUM / SUM_MOD free functions; refactor operator+/- to delegate to statics\n- Add drbg_aes_ctr_t::gen_bits and gen_bn256 with a fast path for near-2^256 moduli; add ecc_point_t::get_coordinates(bn256_t&, bn256_t&) overload\n- Add hash_number_t::mod(int) and drbg_sample_number(seed, int) for deterministic non-cryptographic integer sampling (documented as biased)",
          "timestamp": "2026-05-19T07:36:20-07:00",
          "tree_id": "70d8c822edb8b06bfa2d4a242b348d256cb6fe44",
          "url": "https://github.com/coinbase/cb-mpc/commit/a8ee24c6d48f11d2f61e005e3ed0c5056d42bedc"
        },
        "date": 1779203683725,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.35732535637575463,
            "unit": "us/iter",
            "extra": "iterations: 1959519\ncpu: 0.3573098030690185 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3649703391485497,
            "unit": "us/iter",
            "extra": "iterations: 1920073\ncpu: 0.3649168687857181 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 2.2646864672579174,
            "unit": "us/iter",
            "extra": "iterations: 308947\ncpu: 2.2646264731491152 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3668600908581086,
            "unit": "us/iter",
            "extra": "iterations: 1902306\ncpu: 0.36680679449047604 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.5306810941997,
            "unit": "us/iter",
            "extra": "iterations: 16889\ncpu: 41.526635383977776 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 65.90018095416828,
            "unit": "us/iter",
            "extra": "iterations: 10627\ncpu: 65.8904481979862 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.706867661023097,
            "unit": "us/iter",
            "extra": "iterations: 22125\ncpu: 31.704599322033893 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 65.81960115116387,
            "unit": "us/iter",
            "extra": "iterations: 10598\ncpu: 65.81156567276842 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.959207382877814,
            "unit": "us/iter",
            "extra": "iterations: 35027\ncpu: 19.958638593085357 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.244692174902976,
            "unit": "us/iter",
            "extra": "iterations: 49239\ncpu: 14.243757346818573 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.00330863527659,
            "unit": "us/iter",
            "extra": "iterations: 34996\ncpu: 20.00077868899301 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.3361396019642005,
            "unit": "us/iter",
            "extra": "iterations: 96725\ncpu: 7.332008942879296 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 63.714417561406286,
            "unit": "us/iter",
            "extra": "iterations: 10990\ncpu: 63.70686696997259 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 80.54893949154636,
            "unit": "us/iter",
            "extra": "iterations: 8693\ncpu: 80.54303002415743 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 2.0090624692009063,
            "unit": "us/iter",
            "extra": "iterations: 349036\ncpu: 2.008891695985514 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 66.37512463330324,
            "unit": "us/iter",
            "extra": "iterations: 10567\ncpu: 66.36991482918515 us\nthreads: 1"
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
          "id": "e97eec7b766cf548794656b079e6d3ca52f87a01",
          "message": "fix: bound modulus buffers and OpenSSL install paths (#117)\n\nCap mod_t moduli at 8192 bits before using stack scratch buffers, and add assertions around bounded VLA allocations in modular arithmetic.\n\nHonor CBMPC_OPENSSL_ROOT in the platform OpenSSL build scripts and document native Linux/macOS build targets.\n\nResolves #108",
          "timestamp": "2026-05-22T07:40:14-07:00",
          "tree_id": "bd6376308d9c4237f429c1e6d342bf0b14a6afad",
          "url": "https://github.com/coinbase/cb-mpc/commit/e97eec7b766cf548794656b079e6d3ca52f87a01"
        },
        "date": 1779463090703,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.33070987252000733,
            "unit": "us/iter",
            "extra": "iterations: 2123470\ncpu: 0.33067738607091224 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.34618294116336157,
            "unit": "us/iter",
            "extra": "iterations: 2022295\ncpu: 0.34616707799801716 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 3.153620527069506,
            "unit": "us/iter",
            "extra": "iterations: 222058\ncpu: 3.153359779877329 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.34982086013217534,
            "unit": "us/iter",
            "extra": "iterations: 2002413\ncpu: 0.3498018176070568 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 40.33537590861889,
            "unit": "us/iter",
            "extra": "iterations: 17334\ncpu: 40.33228320064612 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 62.944807778874356,
            "unit": "us/iter",
            "extra": "iterations: 11107\ncpu: 62.941981093004436 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 28.659937443966182,
            "unit": "us/iter",
            "extra": "iterations: 24538\ncpu: 28.657940907979448 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 62.83218559383124,
            "unit": "us/iter",
            "extra": "iterations: 11148\ncpu: 62.831661194833075 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 17.147499852919633,
            "unit": "us/iter",
            "extra": "iterations: 40794\ncpu: 17.146061847330472 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.508959678421817,
            "unit": "us/iter",
            "extra": "iterations: 48262\ncpu: 14.508308317102454 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 17.180447613677742,
            "unit": "us/iter",
            "extra": "iterations: 40774\ncpu: 17.179983935841452 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.037384495652317,
            "unit": "us/iter",
            "extra": "iterations: 98940\ncpu: 7.036830836870839 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 60.71786056644885,
            "unit": "us/iter",
            "extra": "iterations: 11475\ncpu: 60.714351982570804 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 77.88666551723499,
            "unit": "us/iter",
            "extra": "iterations: 8990\ncpu: 77.87827797552825 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 2.922538475671637,
            "unit": "us/iter",
            "extra": "iterations: 239476\ncpu: 2.922346594230733 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 64.29592328163174,
            "unit": "us/iter",
            "extra": "iterations: 10897\ncpu: 64.29181444434253 us\nthreads: 1"
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
          "id": "ba33d3dcbc35f24a65048977c907e9b311428680",
          "message": "fix: harden crypto helpers and add coverage tooling (#121)\n\nSimplify error reporting by removing stack trace emission from ordinary error paths and trimming unused string/error helpers, while keeping API argument validation on structured error returns.\n\nFix several crypto helper edge cases: mod_t assignment now drops stale Montgomery contexts, mod_t::mod(int) handles negative values and INT_MIN without signed overflow, ElGamal /= performs division, and Paillier batch cipher validation runs in an explicit variable-time scope.\n\nTighten ECC behavior by validating curves and signature scalars during ECDSA verification, asserting same-curve point operations, preserving variable-time scalar multiplication when requested, and enforcing Ed25519 point storage alignment.\n\nHarden protocol paths by checking Schnorr nonce points before proof verification, using explicit variable-time evaluation in HD EdDSA derivation, simplifying HD root K-share helpers, and preserving empty access structures during deserialization.\n\nAdd Clang/LLVM coverage configuration and Make targets for text and HTML coverage reports, and ignore generated PDF diff artifacts from internal tooling.",
          "timestamp": "2026-07-08T06:57:56-07:00",
          "tree_id": "cdb97949eff97ff7fcd7e8f501dc48686f17193b",
          "url": "https://github.com/coinbase/cb-mpc/commit/ba33d3dcbc35f24a65048977c907e9b311428680"
        },
        "date": 1783521311346,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3583627535880535,
            "unit": "us/iter",
            "extra": "iterations: 1953858\ncpu: 0.3583261490855528 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.36575118898345743,
            "unit": "us/iter",
            "extra": "iterations: 1915502\ncpu: 0.3657255466191108 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 2.2513739799713237,
            "unit": "us/iter",
            "extra": "iterations: 310653\ncpu: 2.251175552787193 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3711032353302789,
            "unit": "us/iter",
            "extra": "iterations: 1887968\ncpu: 0.37106983381074227 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.41125823235605,
            "unit": "us/iter",
            "extra": "iterations: 16915\ncpu: 41.40987939698493 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 66.00576500516902,
            "unit": "us/iter",
            "extra": "iterations: 10613\ncpu: 66.00167831904265 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 31.82296488513627,
            "unit": "us/iter",
            "extra": "iterations: 21985\ncpu: 31.820555833522853 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 61.06863918155152,
            "unit": "us/iter",
            "extra": "iterations: 11485\ncpu: 61.06445171963438 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.00259759476809,
            "unit": "us/iter",
            "extra": "iterations: 35007\ncpu: 20.000846145056666 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 13.62398452116001,
            "unit": "us/iter",
            "extra": "iterations: 51102\ncpu: 13.623028844272229 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.036069894015817,
            "unit": "us/iter",
            "extra": "iterations: 34910\ncpu: 20.033572071039828 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.211669284457495,
            "unit": "us/iter",
            "extra": "iterations: 97283\ncpu: 7.21092362488821 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 64.18099872636424,
            "unit": "us/iter",
            "extra": "iterations: 10992\ncpu: 64.17714710698692 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 80.09277120291823,
            "unit": "us/iter",
            "extra": "iterations: 8737\ncpu: 80.08709854641185 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 2.000889387490657,
            "unit": "us/iter",
            "extra": "iterations: 350313\ncpu: 2.0006949956182 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 66.62346701041218,
            "unit": "us/iter",
            "extra": "iterations: 10473\ncpu: 66.61475575288836 us\nthreads: 1"
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
          "id": "3a2f7c22b8c54d909accc5278ef8a4519088f160",
          "message": "fix: harden serialization validation and restrict supported platforms (#122)\n\nCrypto hardening:\n- Validate RSA public modulus/exponent on deserialization and bound the public exponent size\n- Bound accept-control tree recursion depth in secret-sharing convert\n- Reject degenerate/non-coprime accumulators before use in the two-Paillier equality proof and range-check the PDL response\n- Fix ecc_point_t deserialization to clear state on a null curve and call the ECDH exec function pointer correctly\n\nBuild system (#106, #120):\n- Drop iOS/Android/WASM support and Xcode helpers; support only 64-bit macOS and Linux, failing fast on unsupported platforms/architectures\n- Always enable stack protector and debug symbols\n\nExpand unit-test coverage across the affected crypto, protocol, and API paths.",
          "timestamp": "2026-07-12T02:23:24-07:00",
          "tree_id": "c625f9916f5c335e2510512c01399197b5d516ec",
          "url": "https://github.com/coinbase/cb-mpc/commit/3a2f7c22b8c54d909accc5278ef8a4519088f160"
        },
        "date": 1783850523922,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3573314366990815,
            "unit": "us/iter",
            "extra": "iterations: 1959436\ncpu: 0.35730099936920623 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3661338338916097,
            "unit": "us/iter",
            "extra": "iterations: 1917459\ncpu: 0.3661291740788198 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 2.454691984497973,
            "unit": "us/iter",
            "extra": "iterations: 285135\ncpu: 2.454353379977905 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3676972398844059,
            "unit": "us/iter",
            "extra": "iterations: 1894196\ncpu: 0.36768322074378773 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.6563916354281,
            "unit": "us/iter",
            "extra": "iterations: 16809\ncpu: 41.65224278660242 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 65.39906532523382,
            "unit": "us/iter",
            "extra": "iterations: 10685\ncpu: 65.39950772110436 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.58006806329895,
            "unit": "us/iter",
            "extra": "iterations: 22053\ncpu: 32.57638085521245 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 61.042666260643564,
            "unit": "us/iter",
            "extra": "iterations: 11494\ncpu: 61.04195867409075 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 20.004229629626327,
            "unit": "us/iter",
            "extra": "iterations: 34965\ncpu: 20.003017074217084 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.172261145085647,
            "unit": "us/iter",
            "extra": "iterations: 49551\ncpu: 14.171438982058877 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 20.10182840711246,
            "unit": "us/iter",
            "extra": "iterations: 34949\ncpu: 20.100325531488746 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.23464167268988,
            "unit": "us/iter",
            "extra": "iterations: 96850\ncpu: 7.23366587506454 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 64.20528961648017,
            "unit": "us/iter",
            "extra": "iterations: 10873\ncpu: 64.20353287961005 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 80.15217937270961,
            "unit": "us/iter",
            "extra": "iterations: 8736\ncpu: 80.14791952838841 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 2.2046877007848966,
            "unit": "us/iter",
            "extra": "iterations: 316882\ncpu: 2.2046539279605653 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 65.87875129484081,
            "unit": "us/iter",
            "extra": "iterations: 10619\ncpu: 65.87315086166302 us\nthreads: 1"
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
          "id": "490d4c20e7cec3982417d398de162b1ad4a2e22e",
          "message": "feat: harden crypto/ZK/protocol primitives and expand test coverage (#123)\n\nCrypto:\n- add bn_t::jacobi and variable-time sum_mul (multi-scalar mul)\n- validate ecurve_t serialization and zero-fill secp256k1 point buffers\n- blind the secp256k1 ecmult gen context and handle equal-point doubling in secp256k1/ed25519 point addition\n- replace ed25519 is_in_subgroup with the point-halving test (eprint 2022/1164) and add fe half/sqrt helpers\n- drop the unused mod_t::b_pow_k_plus1 Barrett field\n- add horner_poly bn256_t and small-scalar variable-time variants\n\nZK:\n- template uc_batch_dl over bn_t/bn256_t so ed25519 uses bn256 arithmetic\n- add bn256_t support to Fischlin transcript hashing\n- use sum_mul in uc_dl verification\n\nProtocol:\n- reject non-positive bitlen in agree_random\n- validate contribution sizes in generate_sid_fixed_mp\n- check curve membership before hashing in the ECDSA-2PC ZK verify",
          "timestamp": "2026-07-15T08:01:28-07:00",
          "tree_id": "3240785428af1d0726825fae5bb83494491fec21",
          "url": "https://github.com/coinbase/cb-mpc/commit/490d4c20e7cec3982417d398de162b1ad4a2e22e"
        },
        "date": 1784129930344,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.32540379573909944,
            "unit": "us/iter",
            "extra": "iterations: 2150622\ncpu: 0.3253713479170212 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.33663712531402346,
            "unit": "us/iter",
            "extra": "iterations: 2072936\ncpu: 0.3366123295654086 us\nthreads: 1"
          },
          {
            "name": "Core/EC/SelfAdd/secp256k1",
            "value": 0.13998452387632243,
            "unit": "us/iter",
            "extra": "iterations: 5007197\ncpu: 0.13996234420175607 us\nthreads: 1"
          },
          {
            "name": "Core/EC/SelfAdd/Ed25519",
            "value": 0.1751219724559171,
            "unit": "us/iter",
            "extra": "iterations: 3985326\ncpu: 0.1751028272216627 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 3.1361262464726116,
            "unit": "us/iter",
            "extra": "iterations: 223230\ncpu: 3.135495484477894 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.33159483497151593,
            "unit": "us/iter",
            "extra": "iterations: 2082428\ncpu: 0.331559836882716 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 39.98769214498164,
            "unit": "us/iter",
            "extra": "iterations: 17492\ncpu: 39.98231077063801 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 62.620559276448404,
            "unit": "us/iter",
            "extra": "iterations: 10780\ncpu: 62.61296196660488 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 29.573051276619896,
            "unit": "us/iter",
            "extra": "iterations: 23617\ncpu: 29.567563450057154 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 57.943166473322634,
            "unit": "us/iter",
            "extra": "iterations: 12068\ncpu: 57.936632830626415 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 17.06230576416751,
            "unit": "us/iter",
            "extra": "iterations: 41012\ncpu: 17.06055588608215 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.731110170917187,
            "unit": "us/iter",
            "extra": "iterations: 47626\ncpu: 14.729709360433372 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 17.078312153821916,
            "unit": "us/iter",
            "extra": "iterations: 40983\ncpu: 17.077272991240243 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.0538830519381825,
            "unit": "us/iter",
            "extra": "iterations: 99386\ncpu: 7.0533467691626575 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 60.29256092091591,
            "unit": "us/iter",
            "extra": "iterations: 11597\ncpu: 60.283627748555624 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 77.66586445380645,
            "unit": "us/iter",
            "extra": "iterations: 9008\ncpu: 77.65536423179387 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 2.9133906179727846,
            "unit": "us/iter",
            "extra": "iterations: 240140\ncpu: 2.913162538519191 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 32.0092281770946,
            "unit": "us/iter",
            "extra": "iterations: 22316\ncpu: 32.00638994443449 us\nthreads: 1"
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
          "id": "23b1ac68670255ce1629b175a242af45f9f07174",
          "message": "fix: correct sign of Paillier-decrypted s in ECDSA-2P signing resulting in a security bug (#124)",
          "timestamp": "2026-07-21T11:50:04-07:00",
          "tree_id": "cd237abad1d771cbabda64536b264ec1afabe5d7",
          "url": "https://github.com/coinbase/cb-mpc/commit/23b1ac68670255ce1629b175a242af45f9f07174"
        },
        "date": 1784662196438,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.40574842959555635,
            "unit": "us/iter",
            "extra": "iterations: 1720735\ncpu: 0.40573975190834155 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.4048925907187688,
            "unit": "us/iter",
            "extra": "iterations: 1737429\ncpu: 0.4048570220711178 us\nthreads: 1"
          },
          {
            "name": "Core/EC/SelfAdd/secp256k1",
            "value": 0.17550056408004439,
            "unit": "us/iter",
            "extra": "iterations: 4011842\ncpu: 0.17512717474915507 us\nthreads: 1"
          },
          {
            "name": "Core/EC/SelfAdd/Ed25519",
            "value": 0.22074004361880276,
            "unit": "us/iter",
            "extra": "iterations: 3172940\ncpu: 0.22072517318323068 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 2.7040126566166,
            "unit": "us/iter",
            "extra": "iterations: 259153\ncpu: 2.70391499616057 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.40667626782691363,
            "unit": "us/iter",
            "extra": "iterations: 1719931\ncpu: 0.4066405012759232 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 46.252231719398964,
            "unit": "us/iter",
            "extra": "iterations: 15139\ncpu: 46.250414096043336 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 75.64195022674603,
            "unit": "us/iter",
            "extra": "iterations: 9262\ncpu: 75.63041772835246 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 36.41667912031598,
            "unit": "us/iter",
            "extra": "iterations: 19325\ncpu: 36.41640781371278 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 69.63570895151167,
            "unit": "us/iter",
            "extra": "iterations: 10043\ncpu: 69.6303148461616 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 21.242064036868392,
            "unit": "us/iter",
            "extra": "iterations: 32981\ncpu: 21.24039598556745 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 15.67792005635087,
            "unit": "us/iter",
            "extra": "iterations: 44719\ncpu: 15.676441065319022 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 21.186173210232855,
            "unit": "us/iter",
            "extra": "iterations: 33035\ncpu: 21.18461232026639 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 8.049167446579622,
            "unit": "us/iter",
            "extra": "iterations: 86762\ncpu: 8.04885208962449 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 70.1929777443596,
            "unit": "us/iter",
            "extra": "iterations: 9975\ncpu: 70.18478917293234 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 92.11159889531129,
            "unit": "us/iter",
            "extra": "iterations: 7604\ncpu: 92.11300604944756 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 2.4522574144755795,
            "unit": "us/iter",
            "extra": "iterations: 285590\ncpu: 2.452108319619035 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 33.38859058467045,
            "unit": "us/iter",
            "extra": "iterations: 20456\ncpu: 33.38828045561197 us\nthreads: 1"
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
          "id": "bf82c0baf8f5f37d0e1937c37544002740873e22",
          "message": "fix: validate R1/R2 points are on curve in ECDSA2P (#125)",
          "timestamp": "2026-07-22T13:25:18-07:00",
          "tree_id": "87911b0d6617e12399e3be4251db083487cd3a89",
          "url": "https://github.com/coinbase/cb-mpc/commit/bf82c0baf8f5f37d0e1937c37544002740873e22"
        },
        "date": 1784754268755,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/EC/Add/secp256k1",
            "value": 0.3575050667687053,
            "unit": "us/iter",
            "extra": "iterations: 1960125\ncpu: 0.35744810407499517 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add/Ed25519",
            "value": 0.3659101377719124,
            "unit": "us/iter",
            "extra": "iterations: 1913162\ncpu: 0.36587505135477294 us\nthreads: 1"
          },
          {
            "name": "Core/EC/SelfAdd/secp256k1",
            "value": 0.1573794728480003,
            "unit": "us/iter",
            "extra": "iterations: 4429197\ncpu: 0.15735317011187344 us\nthreads: 1"
          },
          {
            "name": "Core/EC/SelfAdd/Ed25519",
            "value": 0.20560579590841202,
            "unit": "us/iter",
            "extra": "iterations: 3406058\ncpu: 0.20559940670417245 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/secp256k1",
            "value": 2.2418997920325667,
            "unit": "us/iter",
            "extra": "iterations: 313029\ncpu: 2.241697401838168 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Add_CT/Ed25519",
            "value": 0.3714762044642619,
            "unit": "us/iter",
            "extra": "iterations: 1872347\ncpu: 0.3714515178009201 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/secp256k1",
            "value": 41.264603262465364,
            "unit": "us/iter",
            "extra": "iterations: 16981\ncpu: 41.25581125964312 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply/Ed25519",
            "value": 65.52685754373131,
            "unit": "us/iter",
            "extra": "iterations: 10691\ncpu: 65.5246613039004 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/secp256k1",
            "value": 32.47921681540083,
            "unit": "us/iter",
            "extra": "iterations: 21516\ncpu: 32.47475088306375 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_VT/Ed25519",
            "value": 60.766278063046414,
            "unit": "us/iter",
            "extra": "iterations: 11296\ncpu: 60.76395148725209 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/secp256k1",
            "value": 19.826722485739577,
            "unit": "us/iter",
            "extra": "iterations: 35418\ncpu: 19.824985600542096 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G/Ed25519",
            "value": 14.261059921992173,
            "unit": "us/iter",
            "extra": "iterations: 49481\ncpu: 14.25853543784483 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/secp256k1",
            "value": 19.80165661712761,
            "unit": "us/iter",
            "extra": "iterations: 35325\ncpu: 19.798781684359522 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Multiply_G_VT/Ed25519",
            "value": 7.1905721601342485,
            "unit": "us/iter",
            "extra": "iterations: 97443\ncpu: 7.189661771497188 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/secp256k1",
            "value": 63.465949710047454,
            "unit": "us/iter",
            "extra": "iterations: 11036\ncpu: 63.46383635375133 us\nthreads: 1"
          },
          {
            "name": "Core/EC/MulAdd/Ed25519",
            "value": 80.2596009626388,
            "unit": "us/iter",
            "extra": "iterations: 8726\ncpu: 80.25235296814124 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/secp256k1",
            "value": 2.013346005026684,
            "unit": "us/iter",
            "extra": "iterations: 348125\ncpu: 2.0133068064631963 us\nthreads: 1"
          },
          {
            "name": "Core/EC/Check/Ed25519",
            "value": 33.878420574522586,
            "unit": "us/iter",
            "extra": "iterations: 21026\ncpu: 33.87641168077626 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}