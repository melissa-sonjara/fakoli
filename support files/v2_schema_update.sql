DROP TABLE IF EXISTS `admin_page`;
CREATE TABLE  `admin_page` (
  `admin_page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `identifier` varchar(100) NOT NULL,
  `server_path` varchar(255) NOT NULL,
  `component_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`admin_page_id`),
  KEY `admin_page_identifier_idx` (`identifier`)
) ENGINE=InnoDB AUTO_INCREMENT=47 DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `component`;
CREATE TABLE  `component` (
  `component_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `version` varchar(25) NOT NULL,
  `priority` int(10) unsigned NOT NULL DEFAULT '0',
  `enabled` int(10) unsigned NOT NULL,
  `author` varchar(45) NOT NULL DEFAULT '',
  `description` varchar(300) NOT NULL DEFAULT '',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `component_path` varchar(300) NOT NULL,
  PRIMARY KEY (`component_id`),
  KEY `components_by_name` (`name`,`enabled`)
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=latin1;

ALTER TABLE `news_item` RENAME TO `article`,
 CHANGE COLUMN `news_item_id` `article_id` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
 DROP PRIMARY KEY,
 ADD PRIMARY KEY (`article_id`);

ALTER TABLE `news_item_comment_xref` RENAME TO `article_comment_xref`,
 CHANGE COLUMN `news_item_comment_id` `article_comment_id` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
 CHANGE COLUMN `news_item_id` `article_id` INT(10) UNSIGNED DEFAULT NULL,
 DROP PRIMARY KEY,
 ADD PRIMARY KEY (`article_comment_id`);

ALTER TABLE `news_item_site_xref` RENAME TO `article_site_xref`,
 CHANGE COLUMN `news_item_site_xref_id` `article_site_xref_id` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
 CHANGE COLUMN `news_item_id` `article_id` INT(10) UNSIGNED NOT NULL,
 DROP PRIMARY KEY,
 ADD PRIMARY KEY (`article_site_xref_id`);