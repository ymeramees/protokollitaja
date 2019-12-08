/*
 * andmebaas.h
 *
 *  Created on: 11.11.2008
 *      Author: lauri
 */

#ifndef ANDMEBAAS_H_
#define ANDMEBAAS_H_

class Andmebaas
{
public:
	bool kirjutusabiPuss;
	bool kirjutusabiPustol;
	struct LaskuriNimi{
		QString eesnimi;
		QString perekonnanimi;
		QString sunniaasta;
		QString klubi;
	};
	QList<LaskuriNimi*> nimekiriPuss;
	QList<LaskuriNimi*> nimekiriPustol;
};

#endif /* ANDMEBAAS_H_ */
