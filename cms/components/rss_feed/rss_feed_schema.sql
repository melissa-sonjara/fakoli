-- Component Schema for rss_feed
--
-- Author: Fiifi Baidoo
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.0

CREATE TABLE rss_feed (
  rss_feed_id int(10) unsigned NOT NULL AUTO_INCREMENT,
  rss_title varchar(60) NOT NULL DEFAULT '',
  rss_url varchar(60) NOT NULL DEFAULT '',
  rss_trusted tinyint(3) NOT NULL DEFAULT '0',
  last_updated timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  feed_status tinyint(3) NOT NULL DEFAULT '0',
  PRIMARY KEY (rss_feed_id)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

CREATE TABLE rss_feed_articles (
  rss_article_id int(10) unsigned NOT NULL AUTO_INCREMENT,
  rss_feed_id int(10) unsigned NOT NULL,
  rss_title varchar(60) NOT NULL DEFAULT '',
  rss_article_author varchar(30) NOT NULL DEFAULT '',
  rss_article_newsStory varchar(60) NOT NULL DEFAULT '0',
  date_added timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (rss_article_id)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE rss_feed 
CHANGE COLUMN rss_title rss_title VARCHAR(255) NOT NULL DEFAULT '',
CHANGE COLUMN rss_url rss_url VARCHAR(255) NOT NULL DEFAULT '' ;

ALTER TABLE rss_feed_articles 
CHANGE COLUMN rss_title rss_title VARCHAR(255) NOT NULL DEFAULT '' ,
CHANGE COLUMN rss_article_author rss_article_author VARCHAR(255) NOT NULL DEFAULT '' ,
CHANGE COLUMN rss_article_newsStory rss_article_newsStory LONGTEXT NOT NULL ,
ADD COLUMN article_date DATETIME,
ADD COLUMN rss_article_URL VARCHAR(500) NULL AFTER date_added;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE rss_feed_articles ADD COLUMN publish_status TINYINT(3) NOT NULL DEFAULT 0;

-- END Version 1.2

-- START Version 1.3

ALTER TABLE rss_feed_articles ADD INDEX rss_article_url_idx (rss_article_URL ASC);

-- END Version 1.3

-- START Version 1.4

ALTER TABLE rss_feed_articles ADD COLUMN rss_article_content LONGTEXT NOT NULL ;

-- END Version 1.4

-- START Version 1.5
ALTER TABLE rss_feed_articles ADD COLUMN rss_thumbnail VARCHAR(255) NULL ;
-- END Version 1.5