-- Fakoli Section Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `section` (
  `section_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `section` varchar(255) NOT NULL,
  `default_template` varchar(255) NOT NULL,
  `default_role` varchar(255) DEFAULT NULL,
  `section_title` varchar(255) NOT NULL,
  `site_id` int(10) unsigned NOT NULL DEFAULT '0',
  `use_SSL` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `default_page` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`section_id`)
) ENGINE=InnoDB;

/*!40000 ALTER TABLE `section` DISABLE KEYS */;
INSERT INTO `section` (`section_id`,`section`,`default_template`,`default_role`,`section_title`,`site_id`,`use_SSL`,`default_page`) VALUES 
 (1,'/','default.tpl','','Default Section',1,0,'index');
/*!40000 ALTER TABLE `section` ENABLE KEYS */;

CREATE TABLE `section_content` (
  `section_content_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `section_id` int(10) unsigned NOT NULL,
  `identifier` varchar(255) NOT NULL,
  `template` varchar(255) DEFAULT NULL,
  `role` varchar(255) DEFAULT NULL,
  `use_SSL` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`section_content_id`)
) ENGINE=InnoDB;

/*!40000 ALTER TABLE `section_content` DISABLE KEYS */;
INSERT INTO `section_content` (`section_content_id`,`section_id`,`identifier`,`template`,`role`,`use_SSL`) VALUES 
 (1,1,'index',NULL,NULL,0);
/*!40000 ALTER TABLE `section_content` ENABLE KEYS */;

-- END Version 1.0

-- START Version 1.1

CREATE TABLE  `section_module_xref` (
  `join_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `section_id` int(10) unsigned NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `position` varchar(45) NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  PRIMARY KEY (`join_id`)
) ENGINE=InnoDB;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE `section_content` ADD COLUMN `permissions` varchar(200) NOT NULL DEFAULT '' AFTER `role`;
ALTER TABLE `section` ADD COLUMN `default_permissions` varchar(200) NOT NULL DEFAULT '' AFTER `default_role`;

-- END Version 1.2

-- START Version 1.3

ALTER TABLE section_content ADD COLUMN override_page_title varchar(500) NULL;

-- END Version 1.3