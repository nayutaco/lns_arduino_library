# [Index](index.html)> Backup & Restore DB

## Introduction to backup & restore DB for updating a SD card.

Here, we would like to introduce backup & restore DB for updating a SD card.

1. Backup DB (When this step is in Client mode)
    ⅰ. **Menu: Debug > Backup / Restore DB**  
       ![backup & restore](images/bs01.jpg)
    
    ⅱ. Create a Backup file(lnshield_backup_'hostname'_yyyymmddhms.tar.gz)  
       This step makes 'ptarmd' stop, so you cannot keep using it without rebooting this device.
       ![backup](images/bs02.jpg)

    ⅲ. Download the file  
       You can download the file wherever your local is.  
       When you click 'Download', this function will delete DB of this node. 
       ![backup](images/bs03.jpg)
    
2. Get ready for a new SD card

3. Restore DB (when this step is in AP mode as a first boot)
    
    **[ATTENTION] You must use the latest backup file!**  
    
        If you don't follow below attentions, you have a risk of taken away all of your funds.  
        
            1. After downloading backup, you must not reboot the device and change balance.  
            2. After restoring DB and changing blance, you must not use same backup file to start over.  
            
    ⅰ. **Menu: Debug > Backup / Restore DB**  
       ![backup & restore](images/bs01.jpg)
    
    ⅱ. Click 'Restore'  
       ![backup](images/bs04.jpg)

    ⅲ. Browse the backup file  
       In this step, you have to select the backup file downloaded in step 1-iii.  
       There is no need to give the file name ‘ lnshield_* ’, but its file extension must to be ‘.tar.gz’.
       ![backup](images/bs05.jpg)

    ⅳ. Upload the file  
        Confirm the name you selected is correct and click 'upload' button.
       ![backup](images/bs06.jpg)

    ⅴ. Update DB  
       Click 'Update', this function will update DB used the uploaded file.
       ![upload](images/bs07.jpg)

    ⅵ. Confirm the process complete  
       After the update, you need to reboot this with Client mode. 
       ![complete](images/bs08.jpg)

4. Reboot with Client mode
    1. **Menu: Debug > Reboot > Change Client mode**
   
    2. Confirm 'show channel info' or 'Get 1st layer info' etc.  
       You can confirm it would return to original balance or state.  
