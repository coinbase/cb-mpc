window.BENCHMARK_DATA = {
  "lastUpdate": 1746512434719,
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
      }
    ]
  }
}