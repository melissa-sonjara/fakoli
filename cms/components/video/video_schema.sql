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
) ENGINE=InnoDB;

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
) ENGINE=InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `video` ADD COLUMN `sort_order` INTEGER UNSIGNED DEFAULT 0 AFTER `video_gallery_id`;

-- END Version 1.1

-- START Version 1.2

CREATE TABLE  `video_download` (
  `download_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `video_id` int(10) unsigned NOT NULL,
  `download_date` datetime NOT NULL,
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`download_id`),
  KEY `downloads_by_video` (`video_id`)
) ENGINE=InnoDB;



CREATE OR REPLACe VIEW `video_download_daily_stats` AS select
`vd`.`video_id` AS `video_id`,
`g`.`video_gallery_id` AS `video_gallery_id`,
date_format(`vd`.`download_date`,'%Y-%m-%d') AS `download_date`,
count(1) AS `download_count`

from ((`video_download` `vd` join `video` `v`) join `video_gallery` `g`)

where ((`vd`.`video_id` = `v`.`video_id`) and
(`g`.`video_gallery_id` = `v`.`video_gallery_id`))

group by `vd`.`video_id`,date_format(`vd`.`download_date`,'%Y-%m-%d');




CREATE OR REPLACE VIEW `video_download_total_stats` AS select
`vd`.`video_id` AS `video_id`,
`g`.`video_gallery_id` AS `video_gallery_id`,
count(1) AS `download_count`

from ((`video_download` `vd` join `video` `v`) join `video_gallery` `g`)

where ((`vd`.`video_id` = `v`.`video_id`) and
(`g`.`video_gallery_id` = `v`.`video_gallery_id`))

group by `vd`.`video_id`;

-- END Version 1.2