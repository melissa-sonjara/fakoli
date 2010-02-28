-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	5.1.37-community


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema sonjara_cms
--

CREATE DATABASE IF NOT EXISTS sonjara_cms;
USE sonjara_cms;

--
-- Definition of table `grp`
--

DROP TABLE IF EXISTS `grp`;
CREATE TABLE `grp` (
  `group_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) DEFAULT NULL,
  `Description` text NOT NULL,
  `active` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`group_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

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
  PRIMARY KEY (`group_topic_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

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
  `folder` varchar(200) NOT NULL,
  `date_taken` date DEFAULT NULL,
  `credits` varchar(200) NOT NULL,
  `caption` varchar(200) NOT NULL,
  `keywords` varchar(200) NOT NULL,
  `status` varchar(45) DEFAULT NULL,
  `image_file` varchar(200) NOT NULL,
  `thumbnail` varchar(200) NOT NULL,
  `image_rotator` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `image_type` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`image_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `image`
--

/*!40000 ALTER TABLE `image` DISABLE KEYS */;
/*!40000 ALTER TABLE `image` ENABLE KEYS */;


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
  PRIMARY KEY (`menu_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

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
  `role` varchar(100) NOT NULL,
  `url` varchar(200) NOT NULL,
  `menu_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`menu_item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `menu_item`
--

/*!40000 ALTER TABLE `menu_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `menu_item` ENABLE KEYS */;


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
  PRIMARY KEY (`module_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `module`
--

/*!40000 ALTER TABLE `module` DISABLE KEYS */;
/*!40000 ALTER TABLE `module` ENABLE KEYS */;


--
-- Definition of table `news_item`
--

DROP TABLE IF EXISTS `news_item`;
CREATE TABLE `news_item` (
  `news_item_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL DEFAULT '',
  `message` text NOT NULL,
  `last_modified` datetime DEFAULT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `image_id` int(10) unsigned DEFAULT '0',
  `teaser` varchar(300) NOT NULL,
  `archive_date` date NOT NULL,
  `created_date` datetime NOT NULL,
  `article_type` varchar(200) NOT NULL DEFAULT '',
  PRIMARY KEY (`news_item_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `news_item`
--

/*!40000 ALTER TABLE `news_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `news_item` ENABLE KEYS */;


--
-- Definition of table `news_item_site_xref`
--

DROP TABLE IF EXISTS `news_item_site_xref`;
CREATE TABLE `news_item_site_xref` (
  `news_item_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `news_item_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`news_item_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `news_item_site_xref`
--

/*!40000 ALTER TABLE `news_item_site_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `news_item_site_xref` ENABLE KEYS */;


--
-- Definition of table `page`
--

DROP TABLE IF EXISTS `page`;
CREATE TABLE `page` (
  `page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `page_title` varchar(200) DEFAULT NULL,
  `description` text,
  `author` varchar(45) NOT NULL,
  `created_date` date NOT NULL,
  `edited_date` date NOT NULL,
  `meta_tag_description` varchar(200) DEFAULT NULL,
  `meta_tag_keyword` varchar(200) DEFAULT NULL,
  `role` varchar(100) NOT NULL,
  `published` tinyint(3) unsigned NOT NULL,
  `template` varchar(100) DEFAULT NULL,
  `identifier` varchar(45) NOT NULL,
  `php_code_file` varchar(100) NOT NULL DEFAULT '',
  `site_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`page_id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=158 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `page`
--

/*!40000 ALTER TABLE `page` DISABLE KEYS */;
INSERT INTO `page` (`page_id`,`page_title`,`description`,`author`,`created_date`,`edited_date`,`meta_tag_description`,`meta_tag_keyword`,`role`,`published`,`template`,`identifier`,`php_code_file`,`site_id`) VALUES 
 (5,'Home Page','\r\nYour site content will eventually live here...\r\n\r\n\r\n\r\n\r\n\r\n','Siobhan Green','2010-02-27','2010-02-27','','','',1,'default.tpl','index','',1);
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
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `page_module_xref`
--

/*!40000 ALTER TABLE `page_module_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `page_module_xref` ENABLE KEYS */;


--
-- Definition of table `publication`
--

DROP TABLE IF EXISTS `publication`;
CREATE TABLE `publication` (
  `publication_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(1000) NOT NULL,
  `publication_year` int(10) unsigned NOT NULL,
  `keywords` varchar(1000) NOT NULL,
  `inclusion_criteria` text NOT NULL,
  `intervention` text NOT NULL,
  `outcomes` text NOT NULL,
  `effect` text NOT NULL,
  `short_abstract` text NOT NULL,
  `abstract` text NOT NULL,
  `funding` varchar(500) NOT NULL,
  `journal_id` int(10) unsigned NOT NULL,
  `pubmed_link` varchar(600) NOT NULL DEFAULT '',
  `public` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `regions_other` varchar(200) NOT NULL DEFAULT '',
  `source` varchar(200) NOT NULL DEFAULT '',
  `show_on_home_page` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `code` int(10) unsigned DEFAULT NULL,
  `pdf_file` varchar(200) NOT NULL DEFAULT '',
  PRIMARY KEY (`publication_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `publication`
--

/*!40000 ALTER TABLE `publication` DISABLE KEYS */;
/*!40000 ALTER TABLE `publication` ENABLE KEYS */;


--
-- Definition of table `publication_topic_xref`
--

DROP TABLE IF EXISTS `publication_topic_xref`;
CREATE TABLE `publication_topic_xref` (
  `publication_topic_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `publication_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`publication_topic_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `publication_topic_xref`
--

/*!40000 ALTER TABLE `publication_topic_xref` DISABLE KEYS */;
/*!40000 ALTER TABLE `publication_topic_xref` ENABLE KEYS */;


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
  PRIMARY KEY (`site_id`,`description`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `site`
--

/*!40000 ALTER TABLE `site` DISABLE KEYS */;
INSERT INTO `site` (`site_id`,`site_name`,`domain`,`enabled`,`description`,`default_template`,`print_template`,`popup_template`,`mobile_template`) VALUES 
 (1,'Default Site','',1,'','',NULL,NULL,NULL);
/*!40000 ALTER TABLE `site` ENABLE KEYS */;


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
  `username` varchar(100) NOT NULL,
  `password` varchar(100) NOT NULL,
  `email` varchar(200) NOT NULL,
  `active` tinyint(3) unsigned NOT NULL,
  `clinic_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`user_id`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `site_user`
--

/*!40000 ALTER TABLE `site_user` DISABLE KEYS */;
INSERT INTO `site_user` (`user_id`,`first_name`,`last_name`,`role`,`title`,`username`,`password`,`email`,`active`,`clinic_id`) VALUES 
 (1,'Andrew','Green','admin','Developer','andy','$1$Q54UYxwZ$ALSGuR9KgKPhXIvGPmGFo/','andy@sonjara.com',1,0),
 (2,'Reshma','Joshi','admin','Developer','reshma','$1$3bT.D5CE$Eh1Scu6/T8jCl7yvFLm/d.','reshma@sonjara.com',1,0),
 (3,'Alejandro','Bermudez-Del-Villar','admin','Development Director','alejandro','$1$t9CCUM5c$ZpcjtDT.hL2L6nhVVWLwm.','ABermudez-Del-Villar@ncuih.org',1,0),
 (4,'Siobhan','Green','admin','','sio','$1$gF2.pc1.$2ijgXPz1jPjdKBRDUkCLH1','sio@sonjara.com',1,0),
 (5,'Jesse','Harding','admin','Communications Officer','jharding','$1$Xc3tjfAI$vSHDv24LDJFA0ZfZm9sq5/','jharding@ncuih.org',1,0),
 (6,'Greg','Fine','admin','Webmaster','gfine','$1$Ov3TqmA5$yGrbNwXVTa1VNneP1PdeK1','gfine@ncuih.org',1,0),
 (7,'Donna','Polk-Primm','member','Executive Director','donnap','$1$ZCpacvXm$ypuIxz9DAneQNNl/JN2s0.','dpolk-primm@nuihc.com',1,17),
 (8,'Jami','Bartgis','member','Director of TA & Research','jbartgis','$1$Ck7.uplS$uuN2K58Vd.81EpU8EPkc9.','jbartgis@ncuih.org',0,0),
 (9,'testie','test','member','test','testme','$1$WMxlrMIu$mOxfEUfLbNiFrqOZczSaA.','sio@sonjara.com',0,1),
 (10,'Jo','Namio','admin','','jo','$1$XkYqVEwt$gUJOQ6BSCfRiYHQarYhtg.','jo@sonjara.com',1,0),
 (11,'Ashley','Sargent','admin','','ashley','$1$Pm4.sU0.$eyuMF1qdkQ/.OfbIpkXiu/','ashley@sonjara.com',1,0),
 (12,'','','admin','','Sonjara','$1$Izlu8WDN$HtUnEILFChmsp/3kmaFEF1','',1,0);
/*!40000 ALTER TABLE `site_user` ENABLE KEYS */;


--
-- Definition of table `topic`
--

DROP TABLE IF EXISTS `topic`;
CREATE TABLE `topic` (
  `topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `topic` varchar(100) NOT NULL,
  PRIMARY KEY (`topic_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `topic`
--

/*!40000 ALTER TABLE `topic` DISABLE KEYS */;
/*!40000 ALTER TABLE `topic` ENABLE KEYS */;




/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
