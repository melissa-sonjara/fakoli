-- Fakoli Blog Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `blog` (
  `blog_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL,
  `description` text,
  `owner_id` int(10) unsigned NOT NULL DEFAULT '0',
  `read_access` varchar(400) NOT NULL DEFAULT '',
  `write_access` varchar(400) NOT NULL DEFAULT '',
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `created_date` datetime NOT NULL,
  `identifier` varchar(100) NOT NULL,
  `allow_comments` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `default_article_order` varchar(4) NOT NULL DEFAULT 'DESC',
  `image_id` int(10) unsigned NOT NULL DEFAULT '0',
  `blog_type` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`blog_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `blog` ADD COLUMN `articles_per_page` INT(10) unsigned not null default 10;

-- END Version 1.1
