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

-- START Version 1.1

ALTER TABLE image_gallery ADD COLUMN hidden TINYINT(3) NOT NULL DEFAULT 0;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE image ADD COLUMN related_resource VARCHAR(200) NOT NULL DEFAULT '';

-- END Version 1.2

-- START Version 1.3

ALTER TABLE image_gallery ADD COLUMN thumbnail_size INT(10) NOT NULL DEFAULT 0;
ALTER TABLE image_gallery ADD COLUMN image_link_mode VARCHAR(20) NOT NULL DEFAULT 'None';

-- END Version 1.3

-- START Version 1.4

ALTER TABLE image_gallery ADD COLUMN fix_orientation TINYINT(3) NOT NULL DEFAULT 0;

-- END Version 1.4 