<?php

          
 
$mail = new EmailHandler("janice@sonjara.com", "test iCal", "I sent a test to Andy. I've found I can't open the ics without outlook but I can add to my calendar and can save ics file to drive and import to zimbra using import/export. This seems a bit hard. Not sure this is what you both wanted.", "janice@sonjara.com", "janice gallant");

$mail->send();