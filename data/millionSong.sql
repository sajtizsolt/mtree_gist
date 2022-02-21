--csv header artist;title;tags
-- tags: rock,60###heavy metal,30

DROP INDEX millionSong_gist_index;
DROP TABLE millionSong;
DROP EXTENSION mtree_gist;
SELECT * FROM pg_extension;

CREATE EXTENSION mtree_gist;
SELECT * FROM pg_extension;

CREATE TABLE millionSong (
--  id  SERIAL PRIMARY KEY,
  artist TEXT,
  title	TEXT,
  tags mtree_text_array
);

\copy millionSong FROM '/home/ggombos/mtree/mtree_gist/data/millionSongSrc/minimalmillionSongTags.csv' DELIMITER ';' CSV HEADER;

CREATE INDEX millionSong_gist_index ON millionSong USING gist (tags mtree_text_array_opclass) ;

SET enable_seqscan = off;

-- range query  fingerprint_data_10

SELECT * FROM millionSong ;

SELECT artist, title, (tags <-> 'alternative###100,pop###75,electronic###50,vocal###50,rock###25') AS dst FROM millionSong;
-- order by tags <-> 'alternative###100,pop###75,electronic###50,vocal###50,rock###25';

--SELECT song_fk, hash, offset2, hash_gist, (hash_gist <-> 'xxx,472,171,300,000,000,0000') AS dst FROM fingerprints2 ORDER BY (hash_gist <-> 'xxx,472,171,300,000,000,0000');

 -- song_fk |        hash         | offset2 | dst
-------+---------------------+---------+-----
     -- 169 | 1096972600000000000 |      91 |   0
     -- 169 | 1591826000000000000 |     359 |   5
     -- 169 | 1057585200000000000 |     375 |   6
     -- 169 | 7893881000000000000 |    1194 |   7
     -- 169 | 5911270800000000000 |     578 |   7
     -- 169 | 1226851320000000000 |     614 |   7
     -- 169 | 2351023000000000000 |     767 |   8
     -- 169 | 7721238100000000000 |    1205 |   8
     -- 169 | 6557120000000000000 |     463 |   8
     -- 169 | 4721713000000000000 |    1171 |   8

