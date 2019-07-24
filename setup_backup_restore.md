# [Index](index.html)> Backup & Restore DB

## Introduction to backup & restore DB for updating a SD card.

1. Backup DB (When this step is in Client mode)
    1. **Menu: Debug > Backup / Restore DB**  
       ![backup & restore](images/bs01.jpg)
    
    2. Create a Backup file(lnshield_backup_'hostname'_yyyymmddhms.tar.gz)  
       This step makes 'ptarmd' stop, so you cannot keep using without rebooting this device.
       ![backup](images/bs02.jpg)

    3. Download the file  
       You can download the file wherever your local.   
       When you click this button, this function deletes DB of this node.   
       ![backup](images/bs03.jpg)
    
2. Get ready for a new SD card

3. Restore DB (when this step is in AP mode as First boot)
    
    **[ATTENTION] You must use the latest backup file!**  
    
        If you don't follow below attentions, you have a risk of taken away all of your funds.  
        
            1. After downloading backup, you must not reboot the device and change balance.  
            2. After restoring DB and changing blance, you must not use same backup file to start over.  
            
    1. **Menu: Debug > Backup / Restore DB**  
       ![backup & restore](images/bs01.jpg)
    
    2. Click Restore  
       ![backup](images/bs04.jpg)

    3. Browse the backup file  
       In this step, you have to select the backup file downloaded in step 1-iii.  
       The file name needs not to start from 'lnshield_*', but its file extension must to be '.tar.gz'.  
       ![backup](images/bs05.jpg)

    4. Upload the file  
        Confirm the name you selected is correct and click 'upload' button.  
       ![backup](images/bs06.jpg)

    5. Update DB  
       Click 'update' button, this function will update DB used the uploaded file.
       ![upload](images/bs07.jpg)

    6. Confirm the process complete  
       After the update, you need to reboot this with Client mode.  
       ![complete](images/bs08.jpg)

4. Reboot with Client mode
    1. **Menu: Debug > Reboot > Change Client mode**
   
    2. Confirm 'show channel info' or 'Get 1st layer info' etc.  
       You can confirm it would return to original balance or state.  
