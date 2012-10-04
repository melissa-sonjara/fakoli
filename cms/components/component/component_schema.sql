-- Fakoli Component Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 0 (bootstrap)
-- Tables needed to bootstrap Fakoli - must be loaded before
-- scan:
-- site_role and one record
-- site_user and one record
-- component
-- admin_page
-- component_page
-- component_update_log
-- settings

CREATE TABLE `site_role` (
  `role_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `role` varchar(45) NOT NULL,
  `name` varchar(45) NOT NULL,
  `description` text,
  PRIMARY KEY (`role_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*!40000 ALTER TABLE `site_role` DISABLE KEYS */;
INSERT INTO `site_role` (`role_id`,`role`,`name`,`description`) VALUES 
 (1,'admin','Administrator','Site Administration role.');
/*!40000 ALTER TABLE `site_role` ENABLE KEYS */;

CREATE TABLE `site_user` (
  `user_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `first_name` varchar(100) NOT NULL,
  `last_name` varchar(100) NOT NULL,
  `role` varchar(20) NOT NULL,
  `title` varchar(100) NOT NULL,
  `username` varchar(100) DEFAULT NULL,
  `password` varchar(100) NOT NULL,
  `email` varchar(200) NOT NULL,
  `active` tinyint(3) unsigned NOT NULL,
  `notes` text,
   `language` varchar(60) DEFAULT NULL,
  `composite_class` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;


/*!40000 ALTER TABLE `site_user` DISABLE KEYS */;
INSERT INTO `site_user` (`user_id`,`first_name`,`last_name`,`role`,`title`,`username`,`password`,`email`,`active`,`notes`,`composite_class`) VALUES 
 (1,'Default','Administrator','admin','','admin','$1$57ZTMeZb$cJcpKw7gSe6QT6at2aC37.','andy@sonjara.com',1,NULL,NULL);
/*!40000 ALTER TABLE `site_user` ENABLE KEYS */;

CREATE TABLE `component` (
  `component_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `version` varchar(25) NOT NULL,
  `priority` int(10) unsigned NOT NULL DEFAULT '0',
  `enabled` int(10) unsigned NOT NULL,
  `author` varchar(45) NOT NULL DEFAULT '',
  `description` varchar(300) NOT NULL DEFAULT '',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `component_path` varchar(300) NOT NULL,
  PRIMARY KEY (`component_id`),
  KEY `components_by_name` (`name`,`enabled`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `admin_page` (
  `admin_page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `identifier` varchar(100) NOT NULL,
  `server_path` varchar(255) NOT NULL,
  `component_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`admin_page_id`),
  KEY `admin_page_identifier_idx` (`identifier`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `component_page` (
  `component_page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `identifier` varchar(50) NOT NULL,
  `server_path` varchar(255) NOT NULL,
  `component` varchar(50) NOT NULL,
  `role` varchar(100) NOT NULL DEFAULT '',
  `enabled` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `scanned` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `template` varchar(100) NOT NULL DEFAULT '',
  `page_title` varchar(200) NOT NULL,
  `site_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`component_page_id`),
  KEY `component_page_idx` (`identifier`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;


CREATE TABLE  `settings` (
  `settings_id` int(10) unsigned NOT NULL auto_increment,
  `component` varchar(100) NOT NULL,
  `category` varchar(50) default NULL COMMENT 'optional subsection category under component',
  `name` varchar(100) NOT NULL,
  `annotation` text,
  `options` text COMMENT 'option list select field renderer, if used',
  `value` varchar(500) NOT NULL,
  `field_type` varchar(40) default NULL COMMENT 'Number, String, Boolean, Text, etc.',
  PRIMARY KEY  (`settings_id`),
  KEY `settings_by_name_idx` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE  `component_update_log` (
  `component_update_log_id` int(10) unsigned NOT NULL auto_increment,
  `component` varchar(50) NOT NULL,
  `version_number` double NOT NULL,
  `date_updated` datetime NOT NULL,
  `description` text,
  PRIMARY KEY  USING BTREE (`component_update_log_id`,`version_number`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


-- END Version 0 (bootstrap)


-- START Version 1.0

-- create component table before scan

CREATE TABLE `component_page_module_xref` (
  `join_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `component_page_id` int(10) unsigned NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `position` varchar(45) NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  PRIMARY KEY (`join_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


-- END Version 1.0

-- START Version 1.1

ALTER TABLE `component_update_log` ADD COLUMN `filename` varchar(50) default NULL COMMENT 'output of update scripts - e.g., list of records modified and how modified';

DROP TABLE IF EXISTS `component_update_log_attachment_xref`;
CREATE TABLE  `component_update_log_attachment_xref` (
  `log_xref_id` int(10) unsigned NOT NULL auto_increment,
  `component_update_log_id` int(10) unsigned NOT NULL,
  `attachment_id` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`log_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE `component` MODIFY COLUMN `description` VARCHAR(1000) NOT NULL DEFAULT '';

-- END Version 1.2
