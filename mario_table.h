#ifndef MARIO_TABLE_H_
#define MARIO_TABLE_H_

#define PUT_CONSUMER(row)	(row)->consumer = ((row)->consumer + 1) % (row)->size

#define PUT_PRODUCER(row) (row)->producer = ((row)->producer + 1) % (row)->size
#define GET_CONSUMER(row)	\
		((row)->producer != (row)->consumer) \
				?	&(row)->items[(row)->consumer] : NULL;
#define GET_PRODUCER(row) \
		(((row)->producer + 1) % (row)->size != (row)->consumer) \
						?	&(row)->items[(row)->producer] : NULL;

#define PUT_CONSUMER_T(table, rindex)	\
		(table)->rows[(rindex)].consumer = ((table)->rows[(rindex)].consumer + 1) % (table)->rows[(rindex)].size
#define PUT_PRODUCER_T(table, rindex) \
		(table)->rows[(rindex)].producer = ((table)->rows[(rindex)].producer + 1) % (table)->rows[(rindex)].size
#define GET_CONSUMER_T(table, rindex)	\
		((table)->rows[(rindex)].producer != (table)->rows[(rindex)].consumer) \
				?	&(table)->rows[(rindex)].items[(table)->rows[(rindex)].consumer] : NULL;
#define GET_PRODUCER_T(table, rindex) \
		(((table)->rows[(rindex)].producer + 1) % (table)->rows[(rindex)].size != (table)->rows[(rindex)].consumer ) \
						?	&(table)->rows[(rindex)].items[(table)->rows[(rindex)].producer] : NULL;

#define PUT_CONSUMER_STRITEM(row)	(row)->consumer = ((row)->consumer + 1) % (row)->size
#define PUT_PRODUCER_STRITEM(row) (row)->producer = ((row)->producer + 1) % (row)->size
#define GET_CONSUMER_STRITEM(row)	\
		((row)->producer != (row)->consumer) \
				?	(row)->items[(row)->consumer] : NULL;
#define GET_PRODUCER_STRITEM(row) \
		(((row)->producer + 1) % (row)->size != (row)->consumer) \
						?	(row)->items[(row)->producer] : NULL;
#endif /* MARIO_TABLE_H_ */
