# [Index](index.html)> Lightning Network

Now all [CLIENT mode](setup_faq.md#client-mode) settings are done.  
Let's go to the next step - open channel.

![conn](images/setup_ln_conn.jpg)

* [Open channel from Testnet Faucet (Lightning Shield)](#open-channel-from-testnet-faucet-lightning-shield)
* [Open channel to Testnet Faucet (phone)](#open-channel-to-testnet-faucet-phone)


----

## Open channel from Testnet Faucet (Lightning Shield)

![conn1](images/setup_ln_conn1.jpg)

### 1. Check my node_id

**Menu: Channel > Show Connect Info**  
![show channel](images/ln_testnet01.jpg)

### 2. Connect to other node that you want to open channel with

   Connect Raspberry Pi to peer node and open channel from the peer, because Raspberry Pi might not have a Global IP address.

   1. Decide the peer node  
      You need decide the peer node. Your options are;
      * your node
      * faucet service  
        * Please use [Bitcoin Testnet Lightning Network Faucet](https://faucet.lightning.community/).

   2. Connect to the peer node  
      Currently, Faucet node_id is `0270685ca81a8e4d4d01beec5781f4cc924684072ae52c507f8ebe9daf0caaab7b@159.203.125.125:9735` (please check the faucet site).  
      **Menu: Connect/Close > Connect [NODE_ID@IPADDR:PORT]**  
      ![connect](images/ln_testnet02.jpg)

   3. Check the connection status: **connected**  
      **Menu: Channel > Show Connect Info**  
      ![show channel](images/ln_testnet03.jpg)

### 3. Open channel from the peer node

![testnet faucet](images/ln_testnet04.jpg)

### 4. Check the connection status: **establishing**

**Menu: Channel > Show Connect Info**  
![show channel](images/ln_testnet05.jpg)

### 5. Wait for some confirmation

You can [open channel from phone](#open-channel-to-testnet-faucet-phone) while waiting.

### 6. Check the connection status: **normal operation**  

**Menu: Channel > Show Connect Info**  
![show channel](images/ln_testnet06.jpg)

### 7. Channel is opened

Finally, you can receive Bitcoin with Lightning Network.  
If receiving from hopped node, you will need more confirmation.

## Open channel to Testnet Faucet (phone)

Please use [Eclair Mobile Testnet](https://play.google.com/store/apps/details?id=fr.acinq.eclair.wallet).

![conn2](images/setup_ln_conn2.jpg)

### 1. Setup Eclair Mobile Testnet

1. Install Eclair Mobile Testnet to your Android.
2. Receive Testnet amount

### 2. Open channel to the Testnet Faucet

[Bitcoin Testnet Lightning Network Faucet](https://faucet.lightning.community/)  
![qr](images/testnet_faucet.jpg)

![eclair](images/eclair_testnet_open.jpg)

### 3. Wait for opening channel

wait above 6 confirmations

----

[Next> Payment test](setup_pay.md)

