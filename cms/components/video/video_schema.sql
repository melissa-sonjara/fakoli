-- Fakoli Video Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

DROP TABLE IF EXISTS `video`;
CREATE TABLE `video` (
  `video_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) DEFAULT NULL,
  `video_file` varchar(400) NOT NULL,
  `width` int(10) unsigned NOT NULL DEFAULT '0',
  `height` int(10) unsigned NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `last_modified` datetime NOT NULL,
  `description` text,
  `image_file` varchar(200) DEFAULT NULL,
  `video_gallery_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`video_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `video_gallery`;
CREATE TABLE  `video_gallery` (
  `video_gallery_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gallery_name` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `owner_id` int(10) unsigned NOT NULL DEFAULT '0',
  `keywords` varchar(400) DEFAULT NULL,
  `identifier` varchar(100) DEFAULT NULL,
  `last_modified` datetime NOT NULL,
  `read_access` varchar(500) DEFAULT NULL,
  `write_access` varchar(500) DEFAULT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`video_gallery_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0