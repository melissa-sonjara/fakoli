-- Fakoli Image Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `image_gallery` (
  `gallery_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gallery_name` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `owner_id` int(10) unsigned NOT NULL DEFAULT '0',
  `keywords` varchar(400) DEFAULT NULL,
  `identifier` varchar(100) DEFAULT NULL,
  `last_modified` datetime NOT NULL,
  `read_access` varchar(500) DEFAULT NULL,
  `write_access` varchar(500) DEFAULT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`gallery_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

CREATE TABLE `image` (
  `image_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL,
  `ALT_tag` varchar(100) NOT NULL,
  `date_taken` date DEFAULT NULL,
  `credits` varchar(200) NOT NULL DEFAULT '',
  `caption` varchar(200) NOT NULL DEFAULT '',
  `keywords` varchar(200) NOT NULL DEFAULT '',
  `image_file` varchar(200) NOT NULL DEFAULT '',
  `include_in_slideshow` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `image_type` varchar(50) NOT NULL DEFAULT '',
  `gallery_id` int(10) unsigned NOT NULL DEFAULT '0',
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `owner_id` int(10) unsigned NOT NULL DEFAULT '0',
  `sort_order` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`image_id`)
) ENGINE=InnoDB;

-- END Version 1.0
