-- Fakoli sql needed to create a new website
-- last updated 6/2/2011 Janice Gallant
-- 
-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	5.1.49-1ubuntu8.1


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema - change schema name here as required
--

CREATE DATABASE IF NOT EXISTS fakoli;
USE fakoli;

--
-- Temporary table structure for view `forum_summary`
--
DROP TABLE IF EXISTS `forum_summary`;
DROP VIEW IF EXISTS `forum_summary`;
CREATE TABLE `forum_summary` (
  `forum_id` int(10) unsigned,
  `title` varchar(200),
  `teaser` varchar(300),
  `published` tinyint(3) unsigned,
  `topics` bigint(21),
  `posts` bigint(21)
);

--
-- Definition of table `admin_page`
--

DROP TABLE IF EXISTS `admin_page`;
CREATE TABLE `admin_page` (
  `admin_page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `identifier` varchar(100) NOT NULL,
  `server_path` varchar(255) NOT NULL,
  `component_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`admin_page_id`),
  KEY `admin_page_identifier_idx` (`identifier`)
) ENGINE=InnoDB;

--
-- Dumping data for table `admin_page`
--

/*!40000 ALTER TABLE `admin_page` DISABLE KEYS */;
/*!40000 ALTER TABLE `admin_page` ENABLE KEYS */;


--
-- Definition of table `answer`
--
DROP TABLE IF EXISTS `answer`;
CREATE TABLE  `answer` (
  `answer_id` int(10) unsigned NOT NULL auto_increment,
   `question_id` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
`composite_class` varchar(100) default NULL,
  PRIMARY KEY  (`answer_id`)
) ENGINE=InnoDB COMMENT='Answer to a question';



--
-- Dumping data for table `answer`
--

/*!40000 ALTER TABLE `answer` DISABLE KEYS */;
/*!40000 ALTER TABLE `answer` ENABLE KEYS */;


--
-- Definition of table `article`
--

DROP TABLE IF EXISTS `article`;
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
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `article`
--

/*!40000 ALTER TABLE `article` DISABLE KEYS */;
/*!40000 ALTER TABLE `article` ENABLE KEYS */;


--
-- Definition of table `article_comment_xref`
--

DROP TABLE IF EXISTS `article_comment_xref`;
CREATE TABLE `article_comment_xref` (
  `article_comment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `article_id` int(10) unsigned DEFAULT NULL,
  `comment_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`article_comment_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `article_comment_xref`
--

/*!40000 ALTER TABLE `article_comment_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `article_comment_xref` ENABLE KEYS */;


--
-- Definition of table `article_site_xref`
--

DROP TABLE IF EXISTS `article_site_xref`;
CREATE TABLE `article_site_xref` (
  `article_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `article_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`article_site_xref_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `article_site_xref`
--

/*!40000 ALTER TABLE `article_site_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `article_site_xref` ENABLE KEYS */;


--
-- Definition of table `attachment`
--

DROP TABLE IF EXISTS `attachment`;
CREATE TABLE `attachment` (
  `attachment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `filename` varchar(200) DEFAULT NULL,
  `local_filename` varchar(200) DEFAULT NULL,
  `last_modified` datetime DEFAULT NULL,
  `file_size` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`attachment_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `attachment`
--

/*!40000 ALTER TABLE `attachment` DISABLE KEYS */;
/*!40000 ALTER TABLE `attachment` ENABLE KEYS */;


--
-- Definition of table `authentication_token`
--

DROP TABLE IF EXISTS `authentication_token`;
CREATE TABLE `authentication_token` (
  `authentication_token_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `token` varchar(200) NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `enabled` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `ip_address` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`authentication_token_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `authentication_token`
--

/*!40000 ALTER TABLE `authentication_token` DISABLE KEYS */;
/*!40000 ALTER TABLE `authentication_token` ENABLE KEYS */;


--
-- Definition of table `blog`
--

DROP TABLE IF EXISTS `blog`;
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

--
-- Dumping data for table `blog`
--

/*!40000 ALTER TABLE `blog` DISABLE KEYS */;
/*!40000 ALTER TABLE `blog` ENABLE KEYS */;


--
-- Definition of table `calendar`
--

DROP TABLE IF EXISTS `calendar`;
CREATE TABLE `calendar` (
  `calendar_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  `last_modified` datetime NOT NULL,
  `read_access` varchar(100) DEFAULT NULL,
  `write_access` varchar(100) DEFAULT NULL,
  `identifier` varchar(100) NOT NULL,
  PRIMARY KEY (`calendar_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `calendar`
--

/*!40000 ALTER TABLE `calendar` DISABLE KEYS */;
/*!40000 ALTER TABLE `calendar` ENABLE KEYS */;


--
-- Definition of table `comment`
--

DROP TABLE IF EXISTS `comment`;
CREATE TABLE `comment` (
  `comment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `description` text,
  `date_posted` datetime NOT NULL,
  `title` varchar(100) DEFAULT NULL,
  `user_id` int(10) unsigned DEFAULT NULL,
  `author` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`comment_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `comment`
--

/*!40000 ALTER TABLE `comment` DISABLE KEYS */;
/*!40000 ALTER TABLE `comment` ENABLE KEYS */;


--
-- Definition of table `component`
--

DROP TABLE IF EXISTS `component`;
CREATE TABLE `component` (
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
) ENGINE=InnoDB;

--
-- Dumping data for table `component`
--

/*!40000 ALTER TABLE `component` DISABLE KEYS */;
/*!40000 ALTER TABLE `component` ENABLE KEYS */;


--
-- Definition of table `component_page`
--

DROP TABLE IF EXISTS `component_page`;
CREATE TABLE `component_page` (
  `component_page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `identifier` varchar(50) NOT NULL,
  `server_path` varchar(255) NOT NULL,
  `component` varchar(50) NOT NULL,
  `role` varchar(100) NOT NULL DEFAULT '',
  `enabled` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `scanned` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `template` varchar(100) NOT NULL DEFAULT '',
  `page_title` varchar(200) NOT NULL,
  `site_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`component_page_id`),
  KEY `component_page_idx` (`identifier`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `component_page`
--

/*!40000 ALTER TABLE `component_page` DISABLE KEYS */;
/*!40000 ALTER TABLE `component_page` ENABLE KEYS */;


--
-- Definition of table `component_page_module_xref`
--

DROP TABLE IF EXISTS `component_page_module_xref`;
CREATE TABLE `component_page_module_xref` (
  `join_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `component_page_id` int(10) unsigned NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `position` varchar(45) NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  PRIMARY KEY (`join_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `component_page_module_xref`
--

/*!40000 ALTER TABLE `component_page_module_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `component_page_module_xref` ENABLE KEYS */;


--
-- Definition of table `contact_topic`
--

DROP TABLE IF EXISTS `contact_topic`;
CREATE TABLE `contact_topic` (
  `contact_topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `topic` varchar(100) NOT NULL,
  `recipients` text COMMENT 'one or more admin users to receive email',
  `sort_order` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`contact_topic_id`)
) ENGINE=InnoDB COMMENT='Topics for Contact Us form';

--
-- Dumping data for table `contact_topic`
--

/*!40000 ALTER TABLE `contact_topic` DISABLE KEYS */;
/*!40000 ALTER TABLE `contact_topic` ENABLE KEYS */;


--
-- Definition of table `contact_topic_site_xref`
--

DROP TABLE IF EXISTS `contact_topic_site_xref`;
CREATE TABLE `contact_topic_site_xref` (
  `contact_topic_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `contact_topic_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`contact_topic_site_xref_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `contact_topic_site_xref`
--

/*!40000 ALTER TABLE `contact_topic_site_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `contact_topic_site_xref` ENABLE KEYS */;


--
-- Definition of table `context_help`
--

DROP TABLE IF EXISTS `context_help`;
CREATE TABLE `context_help` (
  `help_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `class_name` varchar(100) NOT NULL,
  `field` varchar(100) NOT NULL,
  `help` text,
  `criteria` text,
  `title` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`help_id`),
  KEY `help_by_class_idx` (`class_name`)
) ENGINE=InnoDB;

--
-- Dumping data for table `context_help`
--

/*!40000 ALTER TABLE `context_help` DISABLE KEYS */;
/*!40000 ALTER TABLE `context_help` ENABLE KEYS */;


--
-- Definition of table `document`
--

DROP TABLE IF EXISTS `document`;
CREATE TABLE `document` (
  `document_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(1000) NOT NULL,
  `publication_date` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `keywords` varchar(1000) DEFAULT NULL,
  `notes` text,
  `public` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `file` varchar(400) NOT NULL,
  `document_library_id` int(10) unsigned NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`document_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `document`
--

/*!40000 ALTER TABLE `document` DISABLE KEYS */;
/*!40000 ALTER TABLE `document` ENABLE KEYS */;


--
-- Definition of table `document_folder`
--

DROP TABLE IF EXISTS `document_folder`;
CREATE TABLE `document_folder` (
  `folder_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `path` varchar(255) NOT NULL,
  `permissions` varchar(100) NOT NULL,
  `document_library_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`folder_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `document_folder`
--

/*!40000 ALTER TABLE `document_folder` DISABLE KEYS */;
/*!40000 ALTER TABLE `document_folder` ENABLE KEYS */;


--
-- Definition of table `document_library`
--

DROP TABLE IF EXISTS `document_library`;
CREATE TABLE `document_library` (
  `document_library_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL,
  `description` text,
  `owner_id` int(10) unsigned NOT NULL DEFAULT '0',
  `manage_folders` varchar(200) NOT NULL DEFAULT '',
  `upload_files` varchar(200) NOT NULL DEFAULT '',
  `delete_files` varchar(200) NOT NULL DEFAULT '',
  `create_date` datetime NOT NULL,
  `last_modified` datetime NOT NULL,
  `allow_access` varchar(200) NOT NULL DEFAULT '',
  `identifier` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`document_library_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `document_library`
--

/*!40000 ALTER TABLE `document_library` DISABLE KEYS */;
/*!40000 ALTER TABLE `document_library` ENABLE KEYS */;


--
-- Definition of table `document_topic_xref`
--

DROP TABLE IF EXISTS `document_topic_xref`;
CREATE TABLE `document_topic_xref` (
  `document_topic_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `document_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  `group_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`document_topic_xref_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `document_topic_xref`
--

/*!40000 ALTER TABLE `document_topic_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `document_topic_xref` ENABLE KEYS */;


--
-- Definition of table `email_template`
--

DROP TABLE IF EXISTS `email_template`;
CREATE TABLE `email_template` (
  `email_template_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL COMMENT 'title of the message type',
  `sender_email` varchar(100) DEFAULT NULL,
  `recipients` text,
  `subject` varchar(100) NOT NULL COMMENT 'email subject line',
  `message` text NOT NULL COMMENT 'text of the message',
  `class_name` varchar(100) NOT NULL,
  PRIMARY KEY (`email_template_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC COMMENT='email messages';

--
-- Dumping data for table `email_template`
--

/*!40000 ALTER TABLE `email_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `email_template` ENABLE KEYS */;


--
-- Definition of table `event`
--

DROP TABLE IF EXISTS `event`;
CREATE TABLE `event` (
  `event_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(255) NOT NULL DEFAULT '',
  `start_date` datetime DEFAULT NULL,
  `end_date` datetime DEFAULT NULL,
  `location` varchar(255) NOT NULL DEFAULT '',
  `description` text NOT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `event_type` varchar(200) NOT NULL,
  `composite_class` varchar(100) DEFAULT NULL,
  `calendar_id` int(10) unsigned DEFAULT '0',
  PRIMARY KEY (`event_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `event`
--

/*!40000 ALTER TABLE `event` DISABLE KEYS */;
/*!40000 ALTER TABLE `event` ENABLE KEYS */;


--
-- Definition of table `event_site_xref`
--

DROP TABLE IF EXISTS `event_site_xref`;
CREATE TABLE `event_site_xref` (
  `event_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`event_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `event_site_xref`
--

/*!40000 ALTER TABLE `event_site_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `event_site_xref` ENABLE KEYS */;


--
-- Definition of table `fileshare_user_xref`
--

DROP TABLE IF EXISTS `fileshare_user_xref`;
CREATE TABLE `fileshare_user_xref` (
  `fileshare_user_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `document_library_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`fileshare_user_xref_id`)
) ENGINE=InnoDB COMMENT='Shares document library acces with users';

--
-- Dumping data for table `fileshare_user_xref`
--

/*!40000 ALTER TABLE `fileshare_user_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `fileshare_user_xref` ENABLE KEYS */;


--
-- Definition of table `forum`
--

DROP TABLE IF EXISTS `forum`;
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

--
-- Dumping data for table `forum`
--

/*!40000 ALTER TABLE `forum` DISABLE KEYS */;
/*!40000 ALTER TABLE `forum` ENABLE KEYS */;


--
-- Definition of table `forum_message`
--

DROP TABLE IF EXISTS `forum_message`;
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

--
-- Dumping data for table `forum_message`
--

/*!40000 ALTER TABLE `forum_message` DISABLE KEYS */;
/*!40000 ALTER TABLE `forum_message` ENABLE KEYS */;


--
-- Definition of table `forum_message_attachment_xref`
--

DROP TABLE IF EXISTS `forum_message_attachment_xref`;
CREATE TABLE `forum_message_attachment_xref` (
  `forum_message_attachment_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `message_id` int(10) unsigned NOT NULL,
  `attachment_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`forum_message_attachment_xref_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `forum_message_attachment_xref`
--

/*!40000 ALTER TABLE `forum_message_attachment_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `forum_message_attachment_xref` ENABLE KEYS */;


--
-- Definition of table `forum_site_xref`
--

DROP TABLE IF EXISTS `forum_site_xref`;
CREATE TABLE `forum_site_xref` (
  `forum_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `forum_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`forum_site_xref_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `forum_site_xref`
--

/*!40000 ALTER TABLE `forum_site_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `forum_site_xref` ENABLE KEYS */;


--
-- Definition of table `forum_subscription`
--

DROP TABLE IF EXISTS `forum_subscription`;
CREATE TABLE `forum_subscription` (
  `forum_subscription_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `forum_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `subscription_type` tinyint(3) unsigned NOT NULL COMMENT 'daily digest or new item notification',
  PRIMARY KEY (`forum_subscription_id`)
) ENGINE=InnoDB COMMENT='Links forums and forum topics to subscribed users';

--
-- Dumping data for table `forum_subscription`
--

/*!40000 ALTER TABLE `forum_subscription` DISABLE KEYS */;
/*!40000 ALTER TABLE `forum_subscription` ENABLE KEYS */;


--
-- Definition of table `forum_topic`
--

DROP TABLE IF EXISTS `forum_topic`;
CREATE TABLE `forum_topic` (
  `topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `message_id` int(10) unsigned NOT NULL,
  `views` int(10) unsigned NOT NULL DEFAULT '0',
  `replies` int(10) unsigned NOT NULL DEFAULT '0',
  `forum_id` int(10) unsigned NOT NULL,
  `date_created` datetime NOT NULL,
  PRIMARY KEY (`topic_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `forum_topic`
--

/*!40000 ALTER TABLE `forum_topic` DISABLE KEYS */;
/*!40000 ALTER TABLE `forum_topic` ENABLE KEYS */;


--
-- Definition of table `grp`
--

DROP TABLE IF EXISTS `grp`;
CREATE TABLE `grp` (
  `group_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) DEFAULT NULL,
  `Description` text NOT NULL,
  `active` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`group_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `grp`
--

/*!40000 ALTER TABLE `grp` DISABLE KEYS */;
/*!40000 ALTER TABLE `grp` ENABLE KEYS */;


--
-- Definition of table `grp_topic_xref`
--

DROP TABLE IF EXISTS `grp_topic_xref`;
CREATE TABLE `grp_topic_xref` (
  `group_topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `group_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`group_topic_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `grp_topic_xref`
--

/*!40000 ALTER TABLE `grp_topic_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `grp_topic_xref` ENABLE KEYS */;


--
-- Definition of table `image`
--

DROP TABLE IF EXISTS `image`;
CREATE TABLE `image` (
  `image_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL,
  `ALT_tag` varchar(100) NOT NULL,
  `date_taken` date DEFAULT NULL,
  `credits` varchar(200) NOT NULL DEFAULT '',
  `caption` varchar(200) NOT NULL DEFAULT '',
  `keywords` varchar(200) NOT NULL DEFAULT '',
  `image_file` varchar(200) NOT NULL DEFAULT '',
  `include_in_slideshow` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `image_type` varchar(50) NOT NULL DEFAULT '',
  `gallery_id` int(10) unsigned NOT NULL DEFAULT '0',
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `owner_id` int(10) unsigned NOT NULL DEFAULT '0',
  `sort_order` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`image_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `image`
--

/*!40000 ALTER TABLE `image` DISABLE KEYS */;
/*!40000 ALTER TABLE `image` ENABLE KEYS */;


--
-- Definition of table `image_gallery`
--

DROP TABLE IF EXISTS `image_gallery`;
CREATE TABLE `image_gallery` (
  `gallery_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gallery_name` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `owner_id` int(10) unsigned NOT NULL DEFAULT '0',
  `keywords` varchar(400) DEFAULT NULL,
  `identifier` varchar(100) DEFAULT NULL,
  `last_modified` datetime NOT NULL,
  `read_access` varchar(500) DEFAULT NULL,
  `write_access` varchar(500) DEFAULT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`gallery_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `image_gallery`
--

/*!40000 ALTER TABLE `image_gallery` DISABLE KEYS */;
/*!40000 ALTER TABLE `image_gallery` ENABLE KEYS */;


--
-- Definition of table `login_token`
--

DROP TABLE IF EXISTS `login_token`;
CREATE TABLE `login_token` (
  `token_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `token` varchar(10) NOT NULL,
  `expire_date` date NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`token_id`),
  UNIQUE KEY `token_idx` (`token`)
) ENGINE=InnoDB;

--
-- Dumping data for table `login_token`
--

/*!40000 ALTER TABLE `login_token` DISABLE KEYS */;
/*!40000 ALTER TABLE `login_token` ENABLE KEYS */;


--
-- Definition of table `menu`
--

DROP TABLE IF EXISTS `menu`;
CREATE TABLE `menu` (
  `menu_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `site_id` int(10) unsigned NOT NULL,
  `name` varchar(200) NOT NULL,
  `identifier` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `css_class` varchar(100) NOT NULL,
  `highlight_current_item` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `highlight_current_section` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`menu_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `menu`
--

/*!40000 ALTER TABLE `menu` DISABLE KEYS */;
/*!40000 ALTER TABLE `menu` ENABLE KEYS */;


--
-- Definition of table `menu_item`
--

DROP TABLE IF EXISTS `menu_item`;
CREATE TABLE `menu_item` (
  `menu_item_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `parent_id` int(10) unsigned NOT NULL,
  `title` varchar(100) DEFAULT NULL,
  `published` tinyint(3) unsigned DEFAULT NULL,
  `page_id` int(10) unsigned NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  `role` varchar(100) NOT NULL DEFAULT '',
  `url` varchar(200) NOT NULL,
  `menu_id` int(10) unsigned NOT NULL,
  `identifier` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`menu_item_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `menu_item`
--

/*!40000 ALTER TABLE `menu_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `menu_item` ENABLE KEYS */;


--
-- Definition of table `menus`
--

DROP TABLE IF EXISTS `menus`;
CREATE TABLE `menus` (
  `menu_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `parent_id` int(10) unsigned NOT NULL,
  `title` varchar(100) DEFAULT NULL,
  `public` tinyint(3) unsigned DEFAULT NULL,
  `page_id` int(10) unsigned NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  `menu_type` varchar(100) NOT NULL,
  `url` varchar(200) NOT NULL,
  PRIMARY KEY (`menu_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `menus`
--

/*!40000 ALTER TABLE `menus` DISABLE KEYS */;
/*!40000 ALTER TABLE `menus` ENABLE KEYS */;


--
-- Definition of table `merge_code`
--

DROP TABLE IF EXISTS `merge_code`;
CREATE TABLE `merge_code` (
  `merge_code_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL COMMENT 'name used in the template',
  `description` text,
  `function` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'whether value_field is a function',
  `map` varchar(80) DEFAULT NULL COMMENT 'how the code maps through relations to the value',
  `class_name` varchar(50) NOT NULL COMMENT 'class name that can map this code',
  PRIMARY KEY (`merge_code_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC COMMENT='email msg map field names to their values';

--
-- Dumping data for table `merge_code`
--

/*!40000 ALTER TABLE `merge_code` DISABLE KEYS */;
/*!40000 ALTER TABLE `merge_code` ENABLE KEYS */;


--
-- Definition of table `module`
--

DROP TABLE IF EXISTS `module`;
CREATE TABLE `module` (
  `module_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL,
  `group_id` int(10) unsigned NOT NULL DEFAULT '0',
  `content_type` varchar(100) DEFAULT NULL,
  `num_items` int(10) unsigned NOT NULL DEFAULT '5',
  `ord_by` varchar(100) DEFAULT NULL,
  `template` text,
  `query_constraint` varchar(200) DEFAULT NULL,
  `php_code_file` varchar(200) DEFAULT NULL,
  `css_class` varchar(200) DEFAULT NULL,
  `menu_id` int(10) unsigned NOT NULL DEFAULT '0',
  `menu_parameters` varchar(200) DEFAULT NULL,
  `global` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `global_position` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`module_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `module`
--

/*!40000 ALTER TABLE `module` DISABLE KEYS */;
/*!40000 ALTER TABLE `module` ENABLE KEYS */;


--
-- Definition of table `page`
--

DROP TABLE IF EXISTS `page`;
CREATE TABLE `page` (
  `page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `page_title` varchar(200) DEFAULT NULL,
  `description` longtext,
  `author` varchar(45) NOT NULL,
  `created_date` date NOT NULL,
  `edited_date` date NOT NULL,
  `meta_tag_description` varchar(200) DEFAULT NULL,
  `meta_tag_keyword` varchar(200) DEFAULT NULL,
  `role` varchar(100) NOT NULL DEFAULT '',
  `published` tinyint(3) unsigned NOT NULL,
  `template` varchar(100) DEFAULT NULL,
  `identifier` varchar(45) NOT NULL,
  `php_code_file` varchar(100) NOT NULL DEFAULT '',
  `site_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`page_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `page`
--

/*!40000 ALTER TABLE `page` DISABLE KEYS */;
INSERT INTO `page` (`page_id`,`page_title`,`description`,`author`,`created_date`,`edited_date`,`meta_tag_description`,`meta_tag_keyword`,`role`,`published`,`template`,`identifier`,`php_code_file`,`site_id`) VALUES 
 (1,'Welcome to Fakoli','Your content will live here.\r\n\r\n','','2011-05-24','2011-05-24','','','',1,'default.tpl','index','',1);
/*!40000 ALTER TABLE `page` ENABLE KEYS */;


--
-- Definition of table `page_module_xref`
--

DROP TABLE IF EXISTS `page_module_xref`;
CREATE TABLE `page_module_xref` (
  `join_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `page_id` int(10) unsigned NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `position` varchar(45) NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  PRIMARY KEY (`join_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `page_module_xref`
--

/*!40000 ALTER TABLE `page_module_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `page_module_xref` ENABLE KEYS */;

DROP TABLE IF EXISTS `paypal_sandbox_transaction`;
CREATE TABLE  `paypal_sandbox_transaction` (
  `transaction_id` int(10) unsigned NOT NULL auto_increment,
  `user_id` int(10) unsigned NOT NULL COMMENT 'references user table',
  `amount` float default NULL,
  `order_date` datetime default NULL COMMENT 'will reflect date order is submitted',
  `payment_transaction_id` varchar(30) default NULL COMMENT 'PayPal transaction id',
  `payment_amount` double default NULL COMMENT 'PayPal payment gross',
  `payment_type` varchar(30) default NULL,
  `status` varchar(30) default NULL,
  PRIMARY KEY  (`transaction_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Definition of table `php_session`
--

DROP TABLE IF EXISTS `php_session`;
CREATE TABLE `php_session` (
  `session_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `data` longtext,
  `expires` int(10) unsigned NOT NULL,
  `id` varchar(200) NOT NULL,
  PRIMARY KEY (`session_id`),
  KEY `session_idx` (`id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `php_session`
--

/*!40000 ALTER TABLE `php_session` DISABLE KEYS */;
/*!40000 ALTER TABLE `php_session` ENABLE KEYS */;


--
-- Definition of table `question`
--

DROP TABLE IF EXISTS `question`;
CREATE TABLE  `question` (
  `question_id` int(10) unsigned NOT NULL auto_increment,
  `questionnaire_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `question_number` int(10) unsigned NOT NULL,
  `question_type_id` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `question_name` varchar(80) default NULL COMMENT 'short question title, for spreadsheet view',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned default '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned default '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned default '0' COMMENT 'number of input rows to display on freetext',
  PRIMARY KEY  (`question_id`)
) ENGINE=InnoDB COMMENT='questions for questionnaires: surveys, applications, etc';


--
-- Dumping data for table `question`
--

/*!40000 ALTER TABLE `question` DISABLE KEYS */;
/*!40000 ALTER TABLE `question` ENABLE KEYS */;


--
-- Definition of table `question_type`
--

DROP TABLE IF EXISTS `question_type`;
CREATE TABLE `question_type` (
  `question_type_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL COMMENT 'type name',
  `class_name` varchar(100) NOT NULL COMMENT 'the name of the question renderer class',
  `sort_order` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'order in the question form drop down',
  `options` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'whether this type has an options list',
  `char_limit` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'whether this type can have a char limit; 0 if not',
  `num_rows` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'default number of rows, if applicable to type; 0 if not',
  PRIMARY KEY (`question_type_id`)
) ENGINE=InnoDB COMMENT='Question types available for questionnaires';

--
-- Dumping data for table `question_type`
--

/*!40000 ALTER TABLE `question_type` DISABLE KEYS */;

/*!40000 ALTER TABLE `question_type` DISABLE KEYS */;

-- Default question types
INSERT INTO `question_type` (`question_type_id`,`name`,`class_name`,`sort_order`,`options`,`char_limit`,`num_rows`) VALUES
 (1,'Multiple Choice','MultipleChoiceView',4,1,0,0),
 (2,'Rating Question','RatingView',5,1,0,0),
 (3,'Short Text','ShortTextView',2,0,0,0),
 (4,'Free Text','FreeTextView',1,0,1,3),
 (5,'Checklist','CheckListView',3,1,0,0),
 (6,'Drop Down List','SelectFieldView',6,1,0,0);
/*!40000 ALTER TABLE `question_type` ENABLE KEYS */;

/*!40000 ALTER TABLE `question_type` ENABLE KEYS */;


--
-- Definition of table `questionnaire`
--

DROP TABLE IF EXISTS `questionnaire`;
CREATE TABLE  `questionnaire` (
  `questionnaire_id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(100) NOT NULL COMMENT 'title of the individiual questionnaire: e.g., Course 312 Evaluation',
  `composite_class` varchar(100) default NULL,
  PRIMARY KEY  (`questionnaire_id`)
) ENGINE=InnoDB COMMENT='user-defined questionnaire';

--
-- Dumping data for table `questionnaire`
--

/*!40000 ALTER TABLE `questionnaire` DISABLE KEYS */;
/*!40000 ALTER TABLE `questionnaire` ENABLE KEYS */;


--
-- Definition of table `section`
--

DROP TABLE IF EXISTS `section`;
CREATE TABLE `section` (
  `section_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `section` varchar(255) NOT NULL,
  `default_template` varchar(255) NOT NULL,
  `default_role` varchar(255) DEFAULT NULL,
  `section_title` varchar(255) NOT NULL,
  `site_id` int(10) unsigned NOT NULL DEFAULT '0',
  `use_SSL` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `default_page` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`section_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `section`
--

/*!40000 ALTER TABLE `section` DISABLE KEYS */;
INSERT INTO `section` (`section_id`,`section`,`default_template`,`default_role`,`section_title`,`site_id`,`use_SSL`,`default_page`) VALUES 
 (1,'/','default.tpl','','Default Section',1,0,'index');
/*!40000 ALTER TABLE `section` ENABLE KEYS */;


--
-- Definition of table `section_content`
--

DROP TABLE IF EXISTS `section_content`;
CREATE TABLE `section_content` (
  `section_content_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `section_id` int(10) unsigned NOT NULL,
  `identifier` varchar(255) NOT NULL,
  `template` varchar(255) DEFAULT NULL,
  `role` varchar(255) DEFAULT NULL,
  `use_SSL` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`section_content_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `section_content`
--

/*!40000 ALTER TABLE `section_content` DISABLE KEYS */;
INSERT INTO `section_content` (`section_content_id`,`section_id`,`identifier`,`template`,`role`,`use_SSL`) VALUES 
 (1,1,'index',NULL,NULL,0);
/*!40000 ALTER TABLE `section_content` ENABLE KEYS */;


--
-- Definition of table `settings`
--

DROP TABLE IF EXISTS `settings`;
CREATE TABLE  `settings` (
  `settings_id` int(10) unsigned NOT NULL auto_increment,
  `component` varchar(100) NOT NULL,
  `category` varchar(50) default NULL COMMENT 'optional subsection category under component',
  `name` varchar(100) NOT NULL,
  `annotation` text,
  `options` text COMMENT 'option list select field renderer, if used',
  `value` varchar(500) NOT NULL,
  `field_type` varchar(40) default NULL COMMENT 'Number, String, Boolean, Text, etc.',
  PRIMARY KEY  (`settings_id`),
  KEY `settings_by_name_idx` (`name`)
) ENGINE=InnoDB;
--
-- Dumping data for table `settings`
--

/*!40000 ALTER TABLE `settings` DISABLE KEYS */;
INSERT INTO `settings` (`settings_id`,`component`,`name`,`annotation`,`value`,`field_type`) VALUES 
 (1,'optimize','optimize_scripts','','','String'),
 (2,'optimize','optimize_styles','','','String'),
 (3,'optimize','compress_javascript','','','String'),
 (4,'paypal','paypal_url','','https://www.sandbox.paypal.com/cgi-bin/webscr','String'),
 (5,'paypal','merchant_id','','DN4KM78LGXRTE','String'),
 (6,'email','use_debugging_mode','','','Boolean'),
 (7,'email','debugging_mode_output_path','','/srv/www/uploads/history.sonjara.com/email_debug_logs','String'),
 (8,'session_persistance','enable_database_persistance','','','Boolean');
/*!40000 ALTER TABLE `settings` ENABLE KEYS */;


--
-- Definition of table `site`
--

DROP TABLE IF EXISTS `site`;
CREATE TABLE `site` (
  `site_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `site_name` varchar(200) NOT NULL,
  `domain` varchar(200) NOT NULL DEFAULT '',
  `enabled` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `description` varchar(300) NOT NULL DEFAULT '',
  `default_template` varchar(200) NOT NULL,
  `print_template` varchar(200) DEFAULT NULL,
  `popup_template` varchar(200) DEFAULT NULL,
  `mobile_template` varchar(200) DEFAULT NULL,
  `home_page` varchar(200) DEFAULT NULL,
  `default_site` tinyint(3) unsigned DEFAULT '0',
  PRIMARY KEY (`site_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `site`
--

/*!40000 ALTER TABLE `site` DISABLE KEYS */;
INSERT INTO `site` (`site_id`,`site_name`,`domain`,`enabled`,`description`,`default_template`,`print_template`,`popup_template`,`mobile_template`,`home_page`,`default_site`) VALUES 
 (1,'Default Web Site','http://www.yourdomain.net',1,'','default.tpl','default.tpl','popup.tpl','default.tpl','/index',1);
/*!40000 ALTER TABLE `site` ENABLE KEYS */;


--
-- Definition of table `site_map`
--

DROP TABLE IF EXISTS `site_map`;
CREATE TABLE `site_map` (
  `site_map_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `identifier` varchar(100) DEFAULT NULL,
  `page_title` varchar(200) NOT NULL DEFAULT '',
  `sort_order` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `role` varchar(100) NOT NULL DEFAULT '',
  `parent_identifier` varchar(100) DEFAULT NULL,
  `published` tinyint(3) unsigned DEFAULT '0' COMMENT 'whether to display this page in a site map view',
  PRIMARY KEY (`site_map_id`)
) ENGINE=InnoDB COMMENT='defines the site hierarchy';

--
-- Dumping data for table `site_map`
--

/*!40000 ALTER TABLE `site_map` DISABLE KEYS */;
/*!40000 ALTER TABLE `site_map` ENABLE KEYS */;


--
-- Definition of table `site_role`
--

DROP TABLE IF EXISTS `site_role`;
CREATE TABLE `site_role` (
  `role_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `role` varchar(45) NOT NULL,
  `name` varchar(45) NOT NULL,
  `description` text,
  PRIMARY KEY (`role_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `site_role`
--

/*!40000 ALTER TABLE `site_role` DISABLE KEYS */;
INSERT INTO `site_role` (`role_id`,`role`,`name`,`description`) VALUES 
 (1,'admin','Administrator','Site Administration role.');
/*!40000 ALTER TABLE `site_role` ENABLE KEYS */;


--
-- Definition of table `site_user`
--

DROP TABLE IF EXISTS `site_user`;
CREATE TABLE `site_user` (
  `user_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `first_name` varchar(100) NOT NULL,
  `last_name` varchar(100) NOT NULL,
  `role` varchar(20) NOT NULL,
  `title` varchar(100) NOT NULL,
  `username` varchar(100) DEFAULT NULL,
  `password` varchar(100) NOT NULL,
  `email` varchar(200) NOT NULL,
  `active` tinyint(3) unsigned NOT NULL,
  `notes` text,
   `language` varchar(60) DEFAULT NULL,
  `composite_class` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`user_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `site_user`
--

/*!40000 ALTER TABLE `site_user` DISABLE KEYS */;
INSERT INTO `site_user` (`user_id`,`first_name`,`last_name`,`role`,`title`,`username`,`password`,`email`,`active`,`notes`,`composite_class`) VALUES 
 (1,'Default','Administrator','admin','','admin','$1$57ZTMeZb$cJcpKw7gSe6QT6at2aC37.','andy@sonjara.com',1,NULL,NULL);
/*!40000 ALTER TABLE `site_user` ENABLE KEYS */;


--
-- Definition of table `survey`
--

DROP TABLE IF EXISTS `survey`;
CREATE TABLE `survey` (
  `survey_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(100) NOT NULL,
  `introduction` text COMMENT 'instructions to users completing survey',
  `start_date` date DEFAULT NULL COMMENT 'date survey request sent',
  `end_date` date DEFAULT NULL COMMENT 'date survey is closed',
  `user_id` int(10) unsigned NOT NULL COMMENT 'references site_user table',
  `sender_email` varchar(100) DEFAULT NULL,
  `recipients` text,
  `message` text,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `deleted` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`survey_id`)
) ENGINE=InnoDB COMMENT='Links a questionnaire to an email template';

--
-- Dumping data for table `survey`
--

/*!40000 ALTER TABLE `survey` DISABLE KEYS */;
/*!40000 ALTER TABLE `survey` ENABLE KEYS */;


--
-- Definition of table `survey_answer`
--

DROP TABLE IF EXISTS `survey_answer`;
CREATE TABLE `survey_answer` (
  `survey_answer_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `response_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire_response table',
  `survey_question_id` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
  PRIMARY KEY (`survey_answer_id`)
) ENGINE=InnoDB COMMENT='Answer to a question';

--
-- Dumping data for table `survey_answer`
--

/*!40000 ALTER TABLE `survey_answer` DISABLE KEYS */;
/*!40000 ALTER TABLE `survey_answer` ENABLE KEYS */;


--
-- Definition of table `survey_question`
--

DROP TABLE IF EXISTS `survey_question`;
CREATE TABLE `survey_question` (
  `survey_question_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `question_type_id` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `question_name` varchar(80) DEFAULT NULL COMMENT 'short question title, for spreadsheet view',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned DEFAULT '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned DEFAULT '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned DEFAULT '0' COMMENT 'number of input rows to display on freetext',
  `locked` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'whether this is a standard, multi-reuse question',
  PRIMARY KEY (`survey_question_id`)
) ENGINE=InnoDB COMMENT='questions for questionnaires: surveys, applications, etc';

--
-- Dumping data for table `survey_question`
--

/*!40000 ALTER TABLE `survey_question` DISABLE KEYS */;
/*!40000 ALTER TABLE `survey_question` ENABLE KEYS */;


--
-- Definition of table `survey_question_xref`
--

DROP TABLE IF EXISTS `survey_question_xref`;
CREATE TABLE `survey_question_xref` (
  `survey_question_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `survey_id` int(10) unsigned DEFAULT NULL COMMENT 'if survey question set',
  `survey_template_id` int(10) unsigned DEFAULT NULL COMMENT 'if standard question set',
  `survey_question_id` int(10) unsigned NOT NULL,
  `sort_order` tinyint(4) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`survey_question_xref_id`)
) ENGINE=InnoDB COMMENT='survey question set or standard set';

--
-- Dumping data for table `survey_question_xref`
--

/*!40000 ALTER TABLE `survey_question_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `survey_question_xref` ENABLE KEYS */;


--
-- Definition of table `survey_response`
--

DROP TABLE IF EXISTS `survey_response`;
CREATE TABLE `survey_response` (
  `response_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `survey_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `token` varchar(10) NOT NULL COMMENT 'assigned response token',
  `email` varchar(100) NOT NULL,
  `last_modified` date DEFAULT NULL COMMENT 'date submitted',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`response_id`),
  UNIQUE KEY `token_idx` (`token`,`survey_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `survey_response`
--

/*!40000 ALTER TABLE `survey_response` DISABLE KEYS */;
/*!40000 ALTER TABLE `survey_response` ENABLE KEYS */;


--
-- Definition of table `survey_setting`
--

-- these settings are now in CMS Application Settings
DROP TABLE IF EXISTS `survey_setting`;


--
-- Definition of table `survey_template`
--

DROP TABLE IF EXISTS `survey_template`;
CREATE TABLE `survey_template` (
  `survey_template_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(100) NOT NULL,
  `introduction` text COMMENT 'instructions to users completing survey',
  `message` text NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'in progress, published, in review',
  PRIMARY KEY (`survey_template_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `survey_template`
--

/*!40000 ALTER TABLE `survey_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `survey_template` ENABLE KEYS */;


--
-- Definition of table `topic`
--

DROP TABLE IF EXISTS `topic`;
CREATE TABLE `topic` (
  `topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `topic` varchar(100) NOT NULL,
  PRIMARY KEY (`topic_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `topic`
--

/*!40000 ALTER TABLE `topic` DISABLE KEYS */;
/*!40000 ALTER TABLE `topic` ENABLE KEYS */;


--
-- Definition of view `forum_summary`
--

DROP TABLE IF EXISTS `forum_summary`;
DROP VIEW IF EXISTS `forum_summary`;
CREATE VIEW `forum_summary` AS select `f`.`forum_id` AS `forum_id`,`f`.`title` AS `title`,`f`.`teaser` AS `teaser`,`f`.`published` AS `published`,(select count(1) AS `count(1)` from `forum_topic` where (`forum_topic`.`forum_id` = `f`.`forum_id`)) AS `topics`,(select count(1) AS `count(1)` from `forum_message` where ((`forum_message`.`forum_id` = `f`.`forum_id`) and (`forum_message`.`deleted` = 0))) AS `posts` from `forum` `f`;



/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
