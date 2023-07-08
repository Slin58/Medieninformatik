#include "Objekt.h"


/*----------------------------------------------------------------------------*/
/* get_normal   gibt den Normalenvektor der Oberflaeche eines Objekts an dem  */
/*   in 'v' bestimmten Punkt zurueck.                                         */
/* Aufrufparameter:    Punkt-Vektor, fuer den der N.-Vektor berechnet werd. s.*/
/*                     Datenstruktur in der der Objektpointer enthalten ist   */
/* Rueckgabeparameter: Berechneter Normalenvektor                             */
/*----------------------------------------------------------------------------*/

Vector Objekt::get_normal(Vector &v)
{
    Vector abc = Vector(surface->a+surface->a, surface->b, surface->c);
    Vector bef = Vector(surface->b, surface->e+surface->e, surface->f);
    Vector cfh = Vector(surface->c, surface->f, surface->h+surface->h);
	Vector normal(
		v.dot(abc) + surface->d,
		v.dot(bef) + surface->g,
		v.dot(cfh) + surface->j);

	return normal.normalize();
} /* get_normal() */

