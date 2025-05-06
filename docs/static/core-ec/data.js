window.BENCHMARK_DATA = {
  "lastUpdate": 1746541577117,
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
      }
    ]
  }
}