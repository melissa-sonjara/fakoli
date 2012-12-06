-- Fakoli Site Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `site` (
  `site_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `site_name` varchar(200) NOT NULL,
  `domain` varchar(200) NOT NULL DEFAULT '',
  `enabled` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `description` varchar(300) NOT NULL DEFAULT '',
  `default_template` varchar(200) NOT NULL,
  `print_template` varchar(200) DEFAULT NULL,
  `popup_template` varchar(200) DEFAULT NULL,
  `mobile_template` varchar(200) DEFAULT NULL,
  `home_page` varchar(200) DEFAULT NULL,
  `default_site` tinyint(3) unsigned DEFAULT '0',
  PRIMARY KEY (`site_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `site`
--

/*!40000 ALTER TABLE `site` DISABLE KEYS */;
INSERT INTO `site` (`site_id`,`site_name`,`domain`,`enabled`,`description`,`default_template`,`print_template`,`popup_template`,`mobile_template`,`home_page`,`default_site`) VALUES 
 (1,'Default Web Site','http://www.yourdomain.net',1,'','default.tpl','default.tpl','popup.tpl','default.tpl','/index',1);
/*!40000 ALTER TABLE `site` ENABLE KEYS */;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `site` ADD COLUMN `meta_tag_description` varchar(200) DEFAULT NULL;
ALTER TABLE `site` ADD COLUMN `meta_tag_keyword` varchar(200) DEFAULT NULL;

-- END Version 1.1
