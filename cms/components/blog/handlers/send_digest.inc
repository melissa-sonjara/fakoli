<?php
/*
 * Description: For each users who subscribes to one or more
 * blogs, send an email with a summary of articles added.
 * 
 * @author: Janice Gallant for Sonjara, Inc.
 * 
 * 5/2013
 */


Fakoli::using("blog", "email", "user", "settings");

$mgr = new BlogSubscriptionManager();

$mgr->sendDigests();
?>