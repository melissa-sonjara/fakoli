DROP TABLE IF EXISTS `attachment`;
CREATE TABLE  `attachment` (
  `attachment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `filename` varchar(200) DEFAULT NULL,
  `local_filename` varchar(200) DEFAULT NULL,
  `last_modified` datetime DEFAULT NULL,
  `file_size` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`attachment_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `event`;
CREATE TABLE  `event` (
  `event_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(255) NOT NULL DEFAULT '',
  `start_date` date DEFAULT NULL,
  `end_date` date DEFAULT NULL,
  `location` varchar(255) NOT NULL DEFAULT '',
  `description` text NOT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `event_type` varchar(200) NOT NULL,
  `composite_class` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`event_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `event_site_xref`;
CREATE TABLE  `event_site_xref` (
  `event_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`event_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `forum`;
CREATE TABLE  `forum` (
  `forum_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `description` text NOT NULL,
  `created_date` datetime NOT NULL,
  `last_modified` datetime NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `title` varchar(200) NOT NULL,
  `teaser` varchar(300) NOT NULL,
  PRIMARY KEY (`forum_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `forum_message`;
CREATE TABLE  `forum_message` (
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
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `forum_message_attachment_xref`;
CREATE TABLE  `forum_message_attachment_xref` (
  `forum_message_attachment_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `message_id` int(10) unsigned NOT NULL,
  `attachment_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`forum_message_attachment_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `forum_site_xref`;
CREATE TABLE  `forum_site_xref` (
  `forum_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `forum_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`forum_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `forum_topic`;
CREATE TABLE  `forum_topic` (
  `topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `message_id` int(10) unsigned NOT NULL,
  `views` int(10) unsigned NOT NULL DEFAULT '0',
  `replies` int(10) unsigned NOT NULL DEFAULT '0',
  `forum_id` int(10) unsigned NOT NULL,
  `date_created` datetime NOT NULL,
  PRIMARY KEY (`topic_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP VIEW IF EXISTS `forum_summary`;
CREATE VIEW  `forum_summary` AS select `f`.`forum_id` AS `forum_id`,`f`.`title` AS `title`,`f`.`teaser` AS `teaser`,`f`.`published` AS `published`,(select count(1) AS `count(1)` from `forum_topic` where (`forum_topic`.`forum_id` = `f`.`forum_id`)) AS `topics`,(select count(1) AS `count(1)` from `forum_message` where ((`forum_message`.`forum_id` = `f`.`forum_id`) and (`forum_message`.`deleted` = 0))) AS `posts` from `forum` `f`;