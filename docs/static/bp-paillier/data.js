window.BENCHMARK_DATA = {
  "lastUpdate": 1761668680636,
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
        "date": 1742577982896,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 50677.50133332538,
            "unit": "us/iter",
            "extra": "iterations: 12\ncpu: 50674.83366666667 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4510.268438711899,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.072058064515 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8584.010170735137,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8582.526256097559 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4524.982877419168,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4524.702199999997 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.027765286831732,
            "unit": "us/iter",
            "extra": "iterations: 63486\ncpu: 11.026958211259174 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 13.040747751883424,
            "unit": "us/iter",
            "extra": "iterations: 54490\ncpu: 13.040027069187001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8316.324238096839,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8315.720809523813 us\nthreads: 1"
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
        "date": 1742998245618,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 58909.9236470461,
            "unit": "us/iter",
            "extra": "iterations: 17\ncpu: 58906.11223529412 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4533.6724155861575,
            "unit": "us/iter",
            "extra": "iterations: 154\ncpu: 4533.391642857143 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8571.411950616033,
            "unit": "us/iter",
            "extra": "iterations: 81\ncpu: 8571.125172839505 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4527.029290323798,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4526.520645161295 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.128012522614132,
            "unit": "us/iter",
            "extra": "iterations: 63006\ncpu: 11.127469320382183 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.850512530064387,
            "unit": "us/iter",
            "extra": "iterations: 54469\ncpu: 12.848788044575821 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8323.063285713162,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8320.815785714292 us\nthreads: 1"
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
        "date": 1746512430114,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 60140.76599999498,
            "unit": "us/iter",
            "extra": "iterations: 10\ncpu: 60136.13710000001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4513.253161290308,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4512.994432258063 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8557.377414634662,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8556.884792682928 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4500.025677419128,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4499.731709677418 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.046545496240894,
            "unit": "us/iter",
            "extra": "iterations: 63225\ncpu: 11.045676615262959 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.78564396641748,
            "unit": "us/iter",
            "extra": "iterations: 54669\ncpu: 12.78454707421024 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8318.898964283844,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8318.350392857137 us\nthreads: 1"
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
        "date": 1746541579765,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 44971.63306250229,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 44965.390250000004 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4514.073954838281,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4513.4220774193545 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8574.59604878009,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8573.51118292683 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4507.924025806314,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4507.347587096777 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.122463114558974,
            "unit": "us/iter",
            "extra": "iterations: 63399\ncpu: 11.12076326125018 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.81618958413544,
            "unit": "us/iter",
            "extra": "iterations: 54609\ncpu: 12.814603087403174 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8374.6170119038,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8373.667238095237 us\nthreads: 1"
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
        "date": 1746544410014,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 49553.290352939846,
            "unit": "us/iter",
            "extra": "iterations: 17\ncpu: 49547.30300000001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4514.022535947327,
            "unit": "us/iter",
            "extra": "iterations: 153\ncpu: 4513.76677124183 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8566.131036587183,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8564.867756097563 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4501.10781290248,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4501.053948387097 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.042495684962136,
            "unit": "us/iter",
            "extra": "iterations: 63383\ncpu: 11.041912089992591 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.795051484204667,
            "unit": "us/iter",
            "extra": "iterations: 54541\ncpu: 12.793535285381639 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8323.17454761605,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8322.434726190468 us\nthreads: 1"
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
        "date": 1746625696908,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 39363.290199995994,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 39360.849733333336 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4511.582109677375,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.954129032257 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8564.847804877456,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8564.55119512195 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4500.486897434724,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4499.959621794873 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.01972305793887,
            "unit": "us/iter",
            "extra": "iterations: 63219\ncpu: 11.018196665559413 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.825125633770496,
            "unit": "us/iter",
            "extra": "iterations: 54436\ncpu: 12.824073774707912 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8319.172440478507,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8318.212035714283 us\nthreads: 1"
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
        "date": 1746628454668,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 45570.44354545393,
            "unit": "us/iter",
            "extra": "iterations: 11\ncpu: 45567.12290909091 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4512.240451614132,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4511.707032258064 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8574.397109752194,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8573.558560975609 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4510.661380643269,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.360432258065 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.023678769942276,
            "unit": "us/iter",
            "extra": "iterations: 63509\ncpu: 11.022226456092834 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.831933150956843,
            "unit": "us/iter",
            "extra": "iterations: 54466\ncpu: 12.831045386112434 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8329.465916663788,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8328.269607142862 us\nthreads: 1"
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
        "date": 1746632564861,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 45065.71627777905,
            "unit": "us/iter",
            "extra": "iterations: 18\ncpu: 45063.23383333333 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4520.631425805644,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4520.337838709676 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8567.43902439054,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8567.00007317073 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4507.8451290318035,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4507.722877419352 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.083187838566044,
            "unit": "us/iter",
            "extra": "iterations: 63134\ncpu: 11.080738112585921 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.879516654410516,
            "unit": "us/iter",
            "extra": "iterations: 54400\ncpu: 12.87914516544119 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8326.565607143537,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8326.29253571429 us\nthreads: 1"
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
        "date": 1746638652969,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 55144.30350001476,
            "unit": "us/iter",
            "extra": "iterations: 12\ncpu: 55138.673 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4517.251470968169,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4516.967690322582 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8574.949625000272,
            "unit": "us/iter",
            "extra": "iterations: 80\ncpu: 8574.485275 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4528.255651612492,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4527.751606451611 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.586435686982167,
            "unit": "us/iter",
            "extra": "iterations: 60431\ncpu: 11.585846469527224 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 13.416013134866756,
            "unit": "us/iter",
            "extra": "iterations: 51999\ncpu: 13.4147881497721 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8317.634654761583,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8317.308440476188 us\nthreads: 1"
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
        "date": 1749827760604,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 57691.18068752732,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 57688.532 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4519.8814387086095,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4519.590941935484 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8568.689060977216,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8568.522707317074 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4618.674664514754,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4618.280922580648 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.579555172817198,
            "unit": "us/iter",
            "extra": "iterations: 60383\ncpu: 11.579317556265837 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 13.398695787118603,
            "unit": "us/iter",
            "extra": "iterations: 52197\ncpu: 13.397940667088161 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8346.721719512638,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8346.399231707323 us\nthreads: 1"
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
        "date": 1749921459593,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 58599.31831578012,
            "unit": "us/iter",
            "extra": "iterations: 19\ncpu: 58591.42536842105 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4515.479529031689,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4514.95895483871 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8558.220646341952,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8557.943134146344 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4516.255116129375,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4515.823574193546 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.012066133941389,
            "unit": "us/iter",
            "extra": "iterations: 63447\ncpu: 11.011125348716257 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.813641250428283,
            "unit": "us/iter",
            "extra": "iterations: 54637\ncpu: 12.81299888354045 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8329.374535714167,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8328.026738095237 us\nthreads: 1"
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
        "date": 1750176292125,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 44687.29235713909,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 44682.55471428572 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4511.142574196011,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.774161290324 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8577.591926828838,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8576.86074390244 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4505.112735484046,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4504.98069032258 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.041194142667242,
            "unit": "us/iter",
            "extra": "iterations: 63237\ncpu: 11.040466467416236 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.89370993450973,
            "unit": "us/iter",
            "extra": "iterations: 54205\ncpu: 12.892964523567928 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8322.761084340285,
            "unit": "us/iter",
            "extra": "iterations: 83\ncpu: 8322.320903614447 us\nthreads: 1"
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
        "date": 1750863418711,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 60341.758374988785,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 60336.51775 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4511.030219354354,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.535535483871 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8564.077170731474,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8563.731743902443 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4507.022993549336,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4506.793587096775 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.572580948434783,
            "unit": "us/iter",
            "extra": "iterations: 60310\ncpu: 11.572097347040286 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 13.425945784746624,
            "unit": "us/iter",
            "extra": "iterations: 52144\ncpu: 13.425365986498923 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8324.878156627274,
            "unit": "us/iter",
            "extra": "iterations: 83\ncpu: 8324.734662650604 us\nthreads: 1"
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
        "date": 1752071928183,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 72881.78312498417,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 72880.14600000001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4515.407954838133,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4515.091516129031 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8565.272768292722,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8564.981414634145 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4504.869870967717,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4504.730238709674 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 10.998039081545143,
            "unit": "us/iter",
            "extra": "iterations: 63585\ncpu: 10.996983124950841 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.818252424504424,
            "unit": "us/iter",
            "extra": "iterations: 54341\ncpu: 12.817521190261488 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8320.895285713787,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8320.114023809516 us\nthreads: 1"
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
        "date": 1752241164448,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 62199.445600003855,
            "unit": "us/iter",
            "extra": "iterations: 10\ncpu: 62197.76250000001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4510.910638710782,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.764703225806 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8575.980914634429,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8575.016109756098 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4513.576200000193,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4513.554290322583 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.125948061621774,
            "unit": "us/iter",
            "extra": "iterations: 63094\ncpu: 11.12464784290106 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.874430180180338,
            "unit": "us/iter",
            "extra": "iterations: 54168\ncpu: 12.87335893885689 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8318.606166667343,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8317.422797619045 us\nthreads: 1"
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
        "date": 1758812360018,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 43825.319999996274,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 43822.04386666666 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4510.019148386663,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4509.548677419354 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8557.754658534954,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8557.307853658534 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4503.321722580539,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4502.91218064516 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.078710488823456,
            "unit": "us/iter",
            "extra": "iterations: 63172\ncpu: 11.077697935794339 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.845788270083846,
            "unit": "us/iter",
            "extra": "iterations: 54442\ncpu: 12.845282575952389 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8333.15214285741,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8332.380904761907 us\nthreads: 1"
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
        "date": 1759938074328,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 50331.43306667019,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 50326.605866666665 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4515.24827096892,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4514.963599999999 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 9108.022999997756,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 9107.37081707317 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4608.709394735917,
            "unit": "us/iter",
            "extra": "iterations: 152\ncpu: 4608.382157894736 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.065716540705079,
            "unit": "us/iter",
            "extra": "iterations: 62718\ncpu: 11.065127236200144 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.890754846067923,
            "unit": "us/iter",
            "extra": "iterations: 54374\ncpu: 12.889821219700595 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8325.17157142493,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8324.78863095238 us\nthreads: 1"
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
        "date": 1760035276822,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 49690.802500013626,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 49688.10964285714 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4512.201290322815,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4511.643761290321 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8569.22930487843,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8568.580304878049 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4507.320864516465,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4507.065864516127 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 10.987369542735182,
            "unit": "us/iter",
            "extra": "iterations: 63814\ncpu: 10.986818503776602 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.824127227441707,
            "unit": "us/iter",
            "extra": "iterations: 54603\ncpu: 12.82322343094702 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8317.024321429688,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8316.789964285712 us\nthreads: 1"
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
        "date": 1760855391236,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 43473.37790909478,
            "unit": "us/iter",
            "extra": "iterations: 22\ncpu: 43469.55677272727 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4513.558864516287,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4513.268941935484 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8567.661182926313,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8566.92564634146 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4508.907967947496,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4508.78461538462 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.002235224827828,
            "unit": "us/iter",
            "extra": "iterations: 63671\ncpu: 11.001579353237737 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.823773949027498,
            "unit": "us/iter",
            "extra": "iterations: 54616\ncpu: 12.822514794199506 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8320.5599880947,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8320.29455952382 us\nthreads: 1"
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
        "date": 1761580217126,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 44946.91742856308,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 44941.23071428571 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4082.6666959059876,
            "unit": "us/iter",
            "extra": "iterations: 171\ncpu: 4082.476853801169 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 7766.219200001009,
            "unit": "us/iter",
            "extra": "iterations: 90\ncpu: 7765.532666666669 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4067.7803255813305,
            "unit": "us/iter",
            "extra": "iterations: 172\ncpu: 4067.6070058139558 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.869051479328718,
            "unit": "us/iter",
            "extra": "iterations: 58878\ncpu: 11.86871148816196 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 13.840671256385463,
            "unit": "us/iter",
            "extra": "iterations: 50693\ncpu: 13.839548024382053 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 7535.566537633771,
            "unit": "us/iter",
            "extra": "iterations: 93\ncpu: 7535.333999999996 us\nthreads: 1"
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
        "date": 1761668680115,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 45044.56126666507,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 45041.003000000004 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4513.363825806723,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4512.71201935484 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8595.48854877974,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8594.289024390244 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4509.449141935494,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4509.236535483869 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.01861591421945,
            "unit": "us/iter",
            "extra": "iterations: 63327\ncpu: 11.017524199788408 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.84105421078141,
            "unit": "us/iter",
            "extra": "iterations: 54491\ncpu: 12.840264869427992 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8326.24830952357,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8325.343904761907 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}