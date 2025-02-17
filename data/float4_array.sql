/* contrib/mtree_gist/data/float4_array.sql */

DROP TABLE IF EXISTS FLOAT4_ARRAY_TEST CASCADE;
CREATE TABLE FLOAT4_ARRAY_TEST (
  id  INTEGER,
  val mtree_float_array
);
COPY FLOAT4_ARRAY_TEST(id, val) FROM '/home/postgres/test_files/float4_array.csv' DELIMITER ';' CSV HEADER;

DROP INDEX IF EXISTS FLOAT4_ARRAY_TEST_IDX CASCADE;
CREATE INDEX FLOAT4_ARRAY_TEST_IDX ON FLOAT4_ARRAY_TEST USING GiST (val mtree_float_array_opclass);

SET enable_seqscan TO OFF;

SELECT * FROM FLOAT4_ARRAY_TEST;

SELECT id, val, (val <-> '0') AS dst FROM FLOAT4_ARRAY_TEST ORDER BY (val <-> '0');

SELECT * FROM FLOAT4_ARRAY_TEST WHERE val #<# '0';
