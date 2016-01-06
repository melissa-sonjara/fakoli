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

-- START Version 1.4

ALTER TABLE `blog_subscriber` add column `subscription_type` varchar(20) NOT NULL DEFAULT 'instant';

-- END Version 1.4

-- START Version 1.5

ALTER TABLE blog ADD COLUMN comment_moderators TEXT;
ALTER TABLE blog ADD COLUMN allow_subscriptions tinyint(3) not null default 0;

UPDATE blog SET allow_subscriptions = (SELECT max(value) FROM settings where component='blog' and name='allow_subscription_options');

ALTER TABLE blog_subscriber ADD COLUMN subscription_token varchar(50);

-- END Version 1.5

-- START Version 1.6

UPDATE blog_subscriber SET subscription_token=(select md5(UUID())) where subscription_token is null;

-- END Version 1.6