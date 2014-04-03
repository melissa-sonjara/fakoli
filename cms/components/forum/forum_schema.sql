-- Fakoli Forum Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `forum` (
  `forum_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `description` text,
  `created_date` datetime NOT NULL,
  `last_modified` datetime DEFAULT NULL,
  `owner_id` int(10) unsigned NOT NULL DEFAULT '0',
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `title` varchar(200) NOT NULL,
  `teaser` varchar(300) DEFAULT NULL,
  PRIMARY KEY (`forum_id`)
) ENGINE=InnoDB;

CREATE TABLE `forum_message` (
  `message_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(300) NOT NULL,
  `message` text NOT NULL,
  `parent_id` int(10) unsigned NOT NULL DEFAULT '0',
  `topic_id` int(10) unsigned NOT NULL DEFAULT '0',
  `author_id` int(10) unsigned NOT NULL,
  `date_posted` datetime NOT NULL,
  `last_modified` datetime NOT NULL,
  `forum_id` int(10) unsigned NOT NULL DEFAULT '0',
  `deleted` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`message_id`)
) ENGINE=InnoDB;

CREATE TABLE `forum_message_attachment_xref` (
  `forum_message_attachment_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `message_id` int(10) unsigned NOT NULL,
  `attachment_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`forum_message_attachment_xref_id`)
) ENGINE=InnoDB;

CREATE TABLE `forum_site_xref` (
  `forum_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `forum_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`forum_site_xref_id`)
) ENGINE=InnoDB;

CREATE TABLE `forum_subscription` (
  `forum_subscription_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `forum_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `subscription_type` tinyint(3) unsigned NOT NULL COMMENT 'daily digest or new item notification',
  PRIMARY KEY (`forum_subscription_id`)
) ENGINE=InnoDB COMMENT='Links forums and forum topics to subscribed users';

CREATE TABLE `forum_topic` (
  `topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `message_id` int(10) unsigned NOT NULL,
  `views` int(10) unsigned NOT NULL DEFAULT '0',
  `replies` int(10) unsigned NOT NULL DEFAULT '0',
  `forum_id` int(10) unsigned NOT NULL,
  `date_created` datetime NOT NULL,
  PRIMARY KEY (`topic_id`)
) ENGINE=InnoDB;

DROP VIEW IF EXISTS `forum_summary`;
CREATE VIEW `forum_summary` AS select `f`.`forum_id` AS `forum_id`,`f`.`title` AS `title`,`f`.`teaser` AS `teaser`,`f`.`published` AS `published`,(select count(1) AS `count(1)` from `forum_topic` where (`forum_topic`.`forum_id` = `f`.`forum_id`)) AS `topics`,(select count(1) AS `count(1)` from `forum_message` where ((`forum_message`.`forum_id` = `f`.`forum_id`) and (`forum_message`.`deleted` = 0))) AS `posts` from `forum` `f`;


-- END Version 1.0

-- START Version 1.1

ALTER TABLE `forum` ADD COLUMN `identifier` VARCHAR(255) NOT NULL DEFAULT '' AFTER `forum_id`;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE forum ADD COLUMN sort_order INT(10) NOT NULL DEFAULT 0;

-- END Version 1.2

-- START Version 1.3

CREATE OR REPLACE VIEW `forum_summary` AS select `f`.`forum_id` AS `forum_id`,`f`.`title` AS `title`,`f`.`teaser` AS `teaser`,`f`.`published` AS `published`, f.sort_order as sort_order, (select count(1) AS `count(1)` from `forum_topic` where (`forum_topic`.`forum_id` = `f`.`forum_id`)) AS `topics`,(select count(1) AS `count(1)` from `forum_message` where ((`forum_message`.`forum_id` = `f`.`forum_id`) and (`forum_message`.`deleted` = 0))) AS `posts` from `forum` `f`;

-- END Version 1.3

-- START Version 1.4

CREATE OR REPLACE VIEW `forum_summary` AS select
`f`.`forum_id` AS `forum_id`,
`f`.`title` AS `title`,
`f`.`teaser` AS `teaser`,
`f`.`published` AS `published`,
`f`.`sort_order` as `sort_order`,

(select count(1) AS `count(1)` from `forum_topic` `ft` where
(`ft`.`forum_id` = `f`.`forum_id` AND `ft`.`message_id` not in
(SELECT `message_id` FROM `forum_message` WHERE `deleted`=1))) AS `topics`,

(select count(1) AS `count(1)` from `forum_message`
where ((`forum_message`.`forum_id` = `f`.`forum_id`)
and (`forum_message`.`deleted` = 0))) AS `posts`

from `forum` `f`;


-- END Version 1.4