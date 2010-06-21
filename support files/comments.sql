DROP TABLE IF EXISTS `comment`;
CREATE TABLE  `comment` (
  `comment_id` int(10) unsigned NOT NULL auto_increment,
  `description` text,
  `date_posted` datetime NOT NULL,
  `title` varchar(100) default NULL,
  `user_id` int(10) unsigned default NULL,
  `author` varchar(45) default NULL,
  PRIMARY KEY  (`comment_id`)
) ENGINE=InnoDB AUTO_INCREMENT=319 DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `news_item_comment_xref`;
CREATE TABLE  `news_item_comment_xref` (
  `news_item_comment_id` int(10) unsigned NOT NULL auto_increment,
  `news_item_id` int(10) unsigned default NULL,
  `comment_id` int(10) unsigned default NULL,
  PRIMARY KEY  (`news_item_comment_id`)
) ENGINE=InnoDB AUTO_INCREMENT=309 DEFAULT CHARSET=latin1;

ALTER TABLE `news_item`
  ADD COLUMN `allow_comment` TINYINT(3) DEFAULT 0,
  ADD COLUMN `headline` TINYINT(3) DEFAULT 0;