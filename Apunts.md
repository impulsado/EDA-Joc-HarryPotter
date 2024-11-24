# TO-DO
- [ ] Algoritme resoldre encanteri.
- [ ] Mirar de com recordar si mag està en procés de conversió.

# CONCEPTES BASE
> ❗ GUANYA QUI MÉS PUNTS TÉ ULTIMA RONDA ❗
``força_magia`` determinar guanyador lluites.
Llibres augmenten força de màgia (Quanta quantitat?)
Puntuació_equip += sum1 + sum2 (Cada ronda) 
sum1 = num_mags_atacats_i_convertits*constant_1
sum2 = quantitat_celes_ocupades*constant_2
Constants definides en `default.cnf`

# FANTASMES
Mouen 8 direccions.
Fantasmes no ataquen a mags ni altres fantasmes.
Pot llegir.

## Encanteris
Vector mida 15 on busquem que tots els subgrups sumin mateixa quantitat.
A cada V[i] ficar el id del grup que pertany (Nou vector) {0,1,2}.
Tots els números <= 30.
Realitzar encanteri esgota fantasma. 
No hi ha encanteris últimes 50 rondes ni inici partida.
15% de les caselles que no siguin parets/ocupades/equip passen a ser nostres.
[//]: Hi havia un examen que feia això o algo similar.

# MAGS
Mouen 4 direccions.
Pot llegir.
Moures cap a un mag meu que es converteix, l'anul·la.
Mags en conversió poden llegir.

## Batalles
Matar a un altre mag, si li treus el doble de força, t'en portes al mag i X punts.
Matar a un altre mag, igualat, inicia fase de conversió. Quan acaba t'en portes al mag i X punts.
Atacar a un mag en conversió no fa res.
Decisió:
0. Si eres qui inicia, amb 30%, guanyes.
1. Tinc JO/(ELL+JO) probabilitat i té ELL/(ELL+JO) probabilitats

# VOLDEMORT
Moure 4 direccions, aura de totes les direccions.
Busca mag/fantasma més proper.
Pot atravessar parets (Cuidado al calcular distància)
Fa desapareixer llibre si està en aura.

# OBERSVACIONS
- Quants més jugadors, menys punts de força.
- Fantasmes son bons per buscar llibres.
- Pot no ser útil sempre resoldre eficient el vector.
- Matar a un altre mag augmenta num_jug, punts, pero també fas que el rival tingui més punts de força per mag.
- Buscar mag al final de rondes per no donar punts al rival.
- Si mag en conversió, que busqui llibres. Si està més aprop que el temps que li queda o sino allunyar-se del llibre.
- No buscar fantasmes que han sigut atacats perque no faig res.
- Pots no donar ordre. Simplement no mouret.
- Ordres a meus jugadors de més urgent a menys.

# ESTRATEGIES
Al principi és millor matar fantasmes i buscar llibres. Això fa tindre màxims punts.
Veure probabilitat que voldemort elimini el llibre que hi ha aprop del mag/fantasma.

# PRIORITATS
0 = Li trec el doble / Hem treuen el doble de punts.


# PREGUNTES
- [ ] Pot reviure un mag?
- [ ] Quantes rondes dura la fase de conversió?
- [ ] Quantes rondes si l'inutilitzo si ataco a un Fantasma?