-- Fakoli Article Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.0
-- Do not include statement to Drop table if exists.
-- If the table exists, we should not be running this
-- sql.

--
-- Definition of table `article`
--

CREATE TABLE `article` (
  `article_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL DEFAULT '',
  `message` longtext NOT NULL,
  `last_modified` datetime DEFAULT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `image_id` int(10) unsigned DEFAULT '0',
  `teaser` text NOT NULL,
  `archive_date` date DEFAULT NULL,
  `created_date` datetime DEFAULT NULL,
  `article_type` varchar(200) NOT NULL DEFAULT '',
  `allow_comment` tinyint(3) DEFAULT '0',
  `headline` tinyint(3) DEFAULT '0',
  `author_id` int(10) unsigned NOT NULL DEFAULT '0',
  `author` varchar(200) DEFAULT NULL,
  `tags` varchar(400) NOT NULL DEFAULT '',
  PRIMARY KEY (`article_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

--
-- Definition of table `article_comment_xref`
--

CREATE TABLE `article_comment_xref` (
  `article_comment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `article_id` int(10) unsigned DEFAULT NULL,
  `comment_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`article_comment_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


--
-- Definition of table `article_site_xref`
--

CREATE TABLE `article_site_xref` (
  `article_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `article_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`article_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `article` ADD COLUMN `sort_order` INT(10) NOT NULL DEFAULT 0;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE `article` ADD COLUMN `publish_date` date DEFAULT NULL AFTER `created_date`;
UPDATE `article` SET `publish_date`=`created_date`;

-- END Version 1.2