-- Fakoli Page Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `page` (
  `page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `page_title` varchar(200) DEFAULT NULL,
  `description` longtext,
  `author` varchar(45) NOT NULL,
  `created_date` date NOT NULL,
  `edited_date` date NOT NULL,
  `meta_tag_description` varchar(200) DEFAULT NULL,
  `meta_tag_keyword` varchar(200) DEFAULT NULL,
  `role` varchar(100) NOT NULL DEFAULT '',
  `published` tinyint(3) unsigned NOT NULL,
  `template` varchar(100) DEFAULT NULL,
  `identifier` varchar(45) NOT NULL,
  `php_code_file` varchar(100) NOT NULL DEFAULT '',
  `site_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`page_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*!40000 ALTER TABLE `page` DISABLE KEYS */;
INSERT INTO `page` (`page_id`,`page_title`,`description`,`author`,`created_date`,`edited_date`,`meta_tag_description`,`meta_tag_keyword`,`role`,`published`,`template`,`identifier`,`php_code_file`,`site_id`) VALUES 
 (1,'Welcome to Fakoli','Your content will live here.\r\n\r\n','','2011-05-24','2011-05-24','','','',1,'default.tpl','index','',1);
/*!40000 ALTER TABLE `page` ENABLE KEYS */;

CREATE TABLE `page_module_xref` (
  `join_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `page_id` int(10) unsigned NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `position` varchar(45) NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  PRIMARY KEY (`join_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0
