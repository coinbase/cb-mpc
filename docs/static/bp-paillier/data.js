window.BENCHMARK_DATA = {
  "lastUpdate": 1746638653389,
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
      }
    ]
  }
}