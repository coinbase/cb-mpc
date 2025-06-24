# Web-based Demo


For the initial setup, first create the certificates:

```bash
make clean-all # deletes all existing certificates, do it only if you want a fresh start
make certs # It will ask you some questions about ca root cert
```

Next, make sure that the c++ library is compiled and installed.

```bash
cd ../../..
make build
sudo make install
```

To run the demo, in four separate terminals, run the following commands:
  - Terminal 1: ` go run *.go -index=0`
  - Terminal 2: ` go run *.go -index=1`
  - Terminal 3: ` go run *.go -index=2`
  - Terminal 4: ` go run *.go -index=3`

And go to the following urls in your browser:
  - [127.0.0.1:7080/page/dkg](127.0.0.1:7080/page/dkg)
  - [127.0.0.1:7081/page/dkg](127.0.0.1:7081/page/dkg)
  - [127.0.0.1:7082/page/dkg](127.0.0.1:7082/page/dkg)
  - [127.0.0.1:7083/page/dkg](127.0.0.1:7083/page/dkg)

