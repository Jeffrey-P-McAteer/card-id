
# Card-ID

CLI tool to encrypt, decrypt, and verify data encrypted or signed using credit cards via a PC smartcard reader.

# Building

```bash
python -m build
```

# Running

```bash
# TODO
```


# Reading

 - https://www.openscdp.org/scripts/tutorial/emv/index.html
    - High-level background of smartcard transactions
 - https://gist.github.com/hemantvallabh/d24d71a933e7319727cd3daa50ad9f2c


# Misc

```bash
pcsc_scan

pkcs11-tool --list-slots
pkcs11-tool --list-objects
pkcs11-tool --login --list-objects

( cd ./build/scsh3.17.584-noinstall/scsh3.17.584/ && chmod +x scsh3 && ./scsh3 )


```

# Research

 - https://iso8583.info/lib/VISA/VIS/APDU
 - https://stackoverflow.com/questions/40663460/use-apdu-commands-to-get-some-information-for-a-card 
