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


-- END Version 1.0

-- START Version 1.1

ALTER TABLE `menu_item` ADD COLUMN `permissions` varchar(200) NOT NULL DEFAULT '' AFTER `role`;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE `menu_item` ADD COLUMN `required_parameters` varchar(200) NOT NULL DEFAULT '';

-- END Version 1.2