#include <vector>
#include "GOAP.h"
#include "Node.h"
#include <iostream>

Node* GOAP::findNodeWithLowestCost(const list<Node*>& nodeList)
{
    if (nodeList.empty()) {
        return nullptr; // Si la liste est vide, retournez nullptr.
    }

    Node* lowestCostNode = nodeList.front(); // Initialisez avec le premier n�ud.
    unsigned int lowestCost = lowestCostNode->unfulfilledCount;

    for (Node* node : nodeList)
    {
        if (node->unfulfilledCount < lowestCost)
        {
            lowestCost = node->unfulfilledCount;
            lowestCostNode = node;
        }
    }

    return lowestCostNode;
}

const unsigned int GOAP::getLowestUnfulfilledCount(const list<Node*>& nodeList)
{
    if (nodeList.empty()) {
        return 0; // Si la liste est vide, il n'y a aucun �l�ment non satisfait.
    }

    unsigned int lowestUnfulfilledCount = numeric_limits<unsigned int>::max(); // Initialisez avec une valeur maximale.

    for (const Node* node : nodeList)
    {
        // V�rifiez le nombre d'�l�ments non satisfaits dans chaque n�ud et mettez � jour le plus bas trouv�.
        unsigned int unfulfilledCount = node->unfulfilledCount;

        if (unfulfilledCount < lowestUnfulfilledCount)
        {
            lowestUnfulfilledCount = unfulfilledCount;
        }
    }

    return lowestUnfulfilledCount;
}

const Node* const GOAP::findBestAction()
{
    list<Node*> openNodes; // Utilisez une liste normale plut�t qu'un pointeur
    openNodes.push_back(new Node(ws->firstAction, nullptr));
    list<EffectCondition>* unfulfilledConditions = new list<EffectCondition>();

    // Initialisez la liste des pr�conditions non remplies avec les pr�conditions de la premi�re action
    for (const pair<EffectCondition, unsigned int>* cond : *ws->firstAction->getConditions()) {
        unfulfilledConditions->push_back(cond->first);
    }

    while (!openNodes.empty())
    {
        // Trouvez le n�ud avec le co�t le plus bas dans openNodes
        Node* currentNode = findNodeWithLowestCost(openNodes);
        cout << currentNode->action->Name << endl;
        // V�rifiez si les pr�conditions de ce n�ud sont satisfaites
        unsigned int unCount = ws->CheckActionCondition(*currentNode->action);
        //cout << currentNode->action->getConditions()->size() << endl;

        if (unCount == 0)
        {
            // Les pr�conditions sont satisfaites, donc ce n�ud est valide directement
            currentNode->cost=0;
            currentNode->unfulfilledCount = 0;
            if(currentNode->prevNode != nullptr)
                currentNode->prevNode->unfulfilledCount -= ws->CheckActionEffect(*currentNode->action);
            ws->Execution(currentNode->action);
            openNodes.remove(currentNode);
            continue;
        }
        else
        {
            //On ajoute les actions 
            currentNode->unfulfilledCount += unCount;
            for (const pair<EffectCondition, unsigned int>* cond : *(currentNode->action->getConditions())) {
                bool t = ws->CheckCondition(cond);
                if (!t) {
                    for (const Action* act : *ws->ActionsList) {
                        for (const pair<EffectCondition, unsigned int>* eff : *(act->getEffects())) {
                            if (eff->first == cond->first) {
                                openNodes.push_back(new Node(act, currentNode));
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Parcourez les effets du n�ud actuel
        for (const pair<EffectCondition, unsigned int>* effect : *(currentNode->action->getEffects())) {
            // Retirez l'effet des pr�conditions non remplies
            if (find(unfulfilledConditions->begin(), unfulfilledConditions->end(), effect->first) != unfulfilledConditions->end()) {
                unfulfilledConditions->remove(effect->first);
                currentNode->unfulfilledCount -= effect->second;
            }
        }

        // Ajoutez les pr�conditions non remplies du n�ud actuel � la liste des pr�conditions � remplir
        for (const pair<EffectCondition, unsigned int>* cond : *(currentNode->action->getConditions())) {
            if(!ws->CheckCondition(cond))
                unfulfilledConditions->push_back(cond->first);
        }

        // �valuez le nombre de pr�conditions non satisfaites
        int unfulfilledCount = unfulfilledConditions->size();

        if (ws->CheckActionCondition(*currentNode->action) <= 0)
            openNodes.remove(currentNode);
    }
    return openNodes.front();
    // � ce stade, openNodes contient les noeuds valides avec les co�ts les plus bas.
    // Vous pouvez maintenant choisir la meilleure action parmi ces n�uds.

    // ...

    // Assurez-vous de g�rer correctement la m�moire en supprimant les n�uds et les listes non n�cessaires.
}