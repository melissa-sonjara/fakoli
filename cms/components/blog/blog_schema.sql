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
) ENGINE=InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `blog` ADD COLUMN `articles_per_page` INT(10) unsigned not null default 10;

-- END Version 1.1

-- START Version 1.2

CREATE TABLE `blog_subscriber` (
  `blog_subscriber_id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `blog_id` INTEGER UNSIGNED NOT NULL DEFAULT 0,
  `subscriber_email` VARCHAR(100) NOT NULL DEFAULT '',
  PRIMARY KEY(`blog_subscriber_id`)
)
ENGINE = InnoDB;

-- END Version 1.2

-- START Version 1.3

ALTER TABLE `blog` ADD COLUMN `enable_rss_feed` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0;
ALTER TABLE `blog` ADD COLUMN `max_rss_articles` INT(10) UNSIGNED NOT NULL DEFAULT 0;

-- END Version 1.3