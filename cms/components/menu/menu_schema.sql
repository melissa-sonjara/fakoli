-- Fakoli Menu Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `menu` (
  `menu_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `site_id` int(10) unsigned NOT NULL,
  `name` varchar(200) NOT NULL,
  `identifier` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `css_class` varchar(100) NOT NULL,
  `highlight_current_item` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `highlight_current_section` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`menu_id`)
) ENGINE=InnoDB;

CREATE TABLE `menu_item` (
  `menu_item_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `parent_id` int(10) unsigned NOT NULL,
  `title` varchar(100) DEFAULT NULL,
  `published` tinyint(3) unsigned DEFAULT NULL,
  `page_id` int(10) unsigned NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  `role` varchar(100) NOT NULL DEFAULT '',
  `url` varchar(200) NOT NULL,
  `menu_id` int(10) unsigned NOT NULL,
  `identifier` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`menu_item_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

/*!40000 ALTER TABLE `menu` DISABLE KEYS */;
INSERT INTO `menu` (`menu_id`,`site_id`,`name`,`identifier`,`description`,`css_class`,`highlight_current_item`,`highlight_current_section`) VALUES 
 (1,1,'global','global','','',0,0);
/*!40000 ALTER TABLE `menu` ENABLE KEYS */;

/*!40000 ALTER TABLE `menu_item` DISABLE KEYS */;
INSERT INTO `menu_item` (`menu_item_id`,`parent_id`,`title`,`published`,`page_id`,`sort_order`,`role`,`url`,`menu_id`,`identifier`) VALUES 
 (1,0,'HOME',1,0,1,'','/index',1,''),
 (2,0,'ADMIN',1,0,2,'','/admin',1,''),
 (3,0,'PAGE',1,0,3,'','/second_page',1,''),
 (4,0,'HELP',1,0,4,'','http://www.fakoli.org/documentation',1,'');
/*!40000 ALTER TABLE `menu_item` ENABLE KEYS */;


-- END Version 1.0

-- START Version 1.1

ALTER TABLE `menu_item` ADD COLUMN `permissions` varchar(200) NOT NULL DEFAULT '' AFTER `role`;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE `menu_item` ADD COLUMN `required_parameters` varchar(200) NOT NULL DEFAULT '';

-- END Version 1.2

-- START Version 1.3

ALTER TABLE menu_item ADD COLUMN custom_format varchar(500) not null default '';

-- END Version 1.3