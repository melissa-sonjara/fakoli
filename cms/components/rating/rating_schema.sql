-- Component Schema for rating
--
-- Author: Andrew Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE rating 
(
	rating_id	int(10) not null auto_increment,
	url			varchar(400) not null default '',
	user_id		int(10) not null default 0,
	rating		float not null default 0.0,
	rating_date	datetime not null,
	PRIMARY KEY(rating_id)
) ENGINE=InnoDB;

CREATE OR REPLACE VIEW rating_statistics AS
SELECT 	url,
		AVG(rating) as average_rating,
		COUNT(1) as number_of_ratings
FROM rating
GROUP BY url;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE rating 
ADD INDEX url_idx (url ASC, user_id ASC);

-- END Version 1.1