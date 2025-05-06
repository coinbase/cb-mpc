window.BENCHMARK_DATA = {
  "lastUpdate": 1746541580201,
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
      }
    ]
  }
}