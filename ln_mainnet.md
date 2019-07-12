# [top](index.html)> Mainnet Lightning Network

Lightning Shield supports Bitcoin testnet and mainnet.  
If you want to use mainnet Lightning Network, change to mainnet and restart.  

* [change to mainnet](#change-to-mainnet)
* [open channel from LNBIG (mainnet)](#open-channel-from-lnbig-mainnet)

----

## Change to mainnet

![img](images/mainnet_testnet.jpg)

## open channel from [LNBIG](https://lnbig.com/#/)

NOTE: use something Linux console.

### 1. change blockchain to mainnet and wait for automatic reboot

**menu: 1st Layer > Change Blockchain Network**  
![change blockchain](images/ln_mainnet01.jpg)

### 2. Mainnet starts

![change blockchain](images/ln_mainnet02.jpg)

### 3. check my node_id

**menu: Channel > Show Connect Info**

### 4. connect to [LNBIG](https://lnbig.com/#/) and call API

1. select "LND / C-Lightning / BTCPay / Eclair" and NEXT.  
![change blockchain](images/ln_mainnet03.jpg)

2. push COPY button on "Commands for LND"
![change blockchain](images/ln_mainnet04.jpg)

3. save the text in `lnbig.txt` on your Linux console

### 5.execute the command below in order to get connection string

```bash
cat lnbig.txt | sed -e 's/^lncli connect \(.*$\)/\1/g' | sed -e 's/^\(.*9735\).*/\1/g'
```

```text
ex.
   028a8e53d70bc0eb7b5660943582f10b7fd6c727a78ad819ba8d45d6a638432c49@lnd-33.LNBIG.com:9735
```

### 6. connect using connection string

**menu: Connect/Close > Connect [NODE_ID@IPADDR:PORT]**

### 7. If the connection is OK, execute the command below to get call the LNBIG API string.

```bash
cat lnbig.txt | sed -e 's/.*| \(.*$\)/\1/g' | sed -e "s/^\(.*private=0'\).*/\1/g"
```

```text
ex.
   curl -G --data-urlencode remoteid@- 'https://lnbig.com/api/v1/oc?k1=040aa501-3273-442c-b966-13488f85e727&private=0'
```

### 8. execute command from Linux console

```bash
echo -n MY_NODEID | LNBIG_API_STRING
```

```text
ex.
   echo -n 03019960f1fe3efb6dd3663606cc196e41ad31fb5535e1264a24d1a1986e7a9059 | curl -G --data-urlencode remoteid@- 'https://lnbig.com/api/v1/oc?k1=040aa501-3273-442c-b966-13488f85e727&private=0'
```

### 9. check LNBIG website

## bash script for LNBIG

If you connect Raspberry Pi via SSH, copy `Commands for LND` strings to `lnbig.txt` and execute the script below.

```bash
#!/bin/sh

CONN_STR=`cat lnbig.txt | sed -e 's/^lncli connect \(.*$\)/\1/g' | sed -e 's/^\(.*9735\).*/\1/g'`
echo $CONN_STR
/home/pi/Prog/ptarmigan/install/ptarmcli --connaddr $CONN_STR


NODEID=`/home/pi/Prog/ptarmigan/install/ptarmcli -l1 | jq -r '.result.node_id'`
API=`cat lnbig.txt | sed -e 's/.*\(https.*$\)/\1/g' | sed -e "s/^\(.*\)&private.*/\1/g"`
echo -n $NODEID | curl -G --data-urlencode remoteid@- "$API&private=0"
```
