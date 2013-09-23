-- Fakoli Site Map Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `site_map` (
  `site_map_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `identifier` varchar(100) DEFAULT NULL,
  `page_title` varchar(200) NOT NULL DEFAULT '',
  `sort_order` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `role` varchar(100) NOT NULL DEFAULT '',
  `parent_identifier` varchar(100) DEFAULT NULL,
  `published` tinyint(3) unsigned DEFAULT '0' COMMENT 'whether to display this page in a site map view',
  PRIMARY KEY (`site_map_id`)
) ENGINE=InnoDB COMMENT='defines the site hierarchy';

-- END Version 1.0

-- START Version 1.1

alter table `site_map` change column `identifier` `url` varchar(200) NOT NULL DEFAULT '';

update `site_map` set `parent_identifier` = '' WHERE parent_identifier IS NULL; 
alter table `site_map` change column `parent_identifier` `parent_url` varchar(200) NOT NULL DEFAULT '';

-- END Version 1.1
