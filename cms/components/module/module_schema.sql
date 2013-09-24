-- Fakoli Module Component schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `module` (
  `module_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL,
  `group_id` int(10) unsigned NOT NULL DEFAULT '0',
  `content_type` varchar(100) DEFAULT NULL,
  `num_items` int(10) unsigned NOT NULL DEFAULT '5',
  `ord_by` varchar(100) DEFAULT NULL,
  `template` text,
  `query_constraint` varchar(200) DEFAULT NULL,
  `php_code_file` varchar(200) DEFAULT NULL,
  `css_class` varchar(200) DEFAULT NULL,
  `menu_id` int(10) unsigned NOT NULL DEFAULT '0',
  `menu_parameters` varchar(200) DEFAULT NULL,
  `global` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `global_position` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`module_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

/*!40000 ALTER TABLE `module` DISABLE KEYS */;
INSERT INTO `module` (`module_id`,`title`,`group_id`,`content_type`,`num_items`,`ord_by`,`template`,`query_constraint`,`php_code_file`,`css_class`,`menu_id`,`menu_parameters`,`global`,`global_position`) VALUES 
 (1,'Footer',0,'HTML',0,'','<p>Fakoli &copy; 2013 Sonjara, Inc. All Rights Reserved</p>\r\n<p>For assistance with Fakoli, please visit <a href=\"http://www.fakoli.org\" target=\"_self\">http://www.fakoli.org</a>,&nbsp; contact <a href=\"mailto:info@sonjara.com\" target=\"_self\">info@sonjara.com</a> or call 571-297-6383</p>','','','',0,'',1,'footer_content');
/*!40000 ALTER TABLE `module` ENABLE KEYS */;

-- END Version 1.0
