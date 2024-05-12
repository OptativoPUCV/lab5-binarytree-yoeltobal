#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

//retorna 1 si son iguales y 0 si no lo son
int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {

  TreeMap *map = (TreeMap *) malloc(sizeof(TreeMap));

  map->root = NULL;
  map->current = NULL;
  map->lower_than = lower_than;

  return map;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) {

  if (tree == NULL) return;

  //busca si la clave ya existe en el mapa
  void* aux = searchTreeMap(tree, key);
  //la llave ya estaba en el mapa
  if (aux != NULL) return;

  
  tree->current = tree->root;
  
  //busca donde insertar
  while (tree->current != NULL) {
    if (tree->lower_than(key,
      tree->current->pair->key) == 1) {
      //la clave es menor al current
      //se mueve a la izquierda
      if (tree->current->left == NULL) {
        //el hijo izquierdo es nulo
        TreeNode * newNode = createTreeNode(key, value);
        tree->current->left = newNode;
        tree->current->left->parent = tree->current;
        //actualiza el current al nuevo nodo inserta2
        tree->current = tree->current->left;
        return; 
      }

      tree->current = tree->current->left;
      
    } else {
        //la clave es mayor o igual al current
        //se mueve a la derecha
        if (tree->current->right == NULL) {
          //el hijo derecho es nulo
          TreeNode * newNode = createTreeNode(key, value);
          tree->current->right = newNode;
          tree->current->right->parent = tree->current;
          
          tree->current = tree->current->right;
          return;
        }
        tree->current = tree->current->right;
    }
    
  }

  //si el árbol estaba vacío se inserta en el root
  tree->root = createTreeNode(key, value);
  //actualizar el current 
  tree->current = tree->root;
}


TreeNode * minimum(TreeNode * x){

  while (x->left != NULL) {
    x = x->left;
  }

  return x;
}


void removeNode(TreeMap * tree, TreeNode* node) {
  
  if (tree == NULL || node == NULL) return;

  //nodo sin hijos
  if (node->left == NULL && node->right == NULL) {
    
    if (node == tree->root) tree->root = NULL;
    else {
      // Si no es la raíz, elimina la referencia al nodo           del padre
      if (node->parent->left == node)
        node->parent->left = NULL;
      else node->parent->right = NULL;
          
    }
  
  }
    
  //nodo con un hijo
    
  else if (node->left == NULL || node->right == NULL) {
    //si node->left es diferente de null, hijo será
    //el nodo izquierdo, sino será el derecho
    TreeNode* hijo = (node->left != NULL) 
    ? node->left : node->right;
    
    //actualiza el padre del hijo
    if (node->parent == NULL) tree->root = hijo;
    else {
          
      if (node->parent->left == node) 
        node->parent->left = hijo;
      else node->parent->right = hijo;
          
    }
    // Actualiza el padre del hijo
    hijo->parent = node->parent;

    
  }
    
  //nodo con dos hijos
    
  else {
    //nodo mínimo en el subárbol derecho
    TreeNode* minNode = minimum(node->right);
    //reemplazar los datos del nodo a eliminar con los           del nodo mínimo
    node->pair = minNode->pair;
    //eliminar el nodo mínimo (que ahora contiene los           datos del nodo a eliminar)
    removeNode(tree, minNode);
  }
  
}

void eraseTreeMap(TreeMap * tree, void* key){
  if (tree == NULL || tree->root == NULL) return;

  if (searchTreeMap(tree, key) == NULL) return;
  TreeNode* node = tree->current;
  removeNode(tree, node);

}




Pair * searchTreeMap(TreeMap * tree, void* key) {
  
  tree->current = tree->root;

  while(tree->current != NULL) {

    // si las claves son iguales, se encuentra el par
    if (is_equal(tree ,tree->current->pair->key,key)) {
      Pair * par = tree->current->pair;
      return par;
    }

    //clave 1 > clave 2
    if (tree->lower_than(key, 
      tree->current->pair->key) == 1) {
      tree->current = tree->current->left;
    } else {
      tree->current = tree->current->right;
    }
    
  }
  
  return NULL;
}


Pair * upperBound(TreeMap * tree, void* key) {

  Pair *par = searchTreeMap(tree,key);
  if (par != NULL) return par;
  
  TreeNode *ub_node = NULL; 
  tree->current = tree->root;

  while (tree->current != NULL) {
    if (tree->lower_than(tree->current->pair->key, 
      key) == 1) {
  
      tree->current = tree->current->right;
    } else {
      
      ub_node = tree->current;
      tree->current = tree->current->left;
    }
  }

  return (ub_node != NULL) ? ub_node->pair : NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
  
  Pair *par = searchTreeMap(tree, 
  (void *)minimum(tree->root)->pair->key);
  
  return par;
}

Pair * nextTreeMap(TreeMap * tree) {
  

  if(tree->current->right != NULL) {
    tree->current = minimum(tree->current->right);  
    return tree->current->pair;
  }

  TreeNode* parent = tree->current->parent;
  while(parent != NULL && tree->current == parent->right){

    tree->current = parent;
    parent = parent->parent;
    
  }
  
  tree->current = parent;
  
  if(parent != NULL) return parent->pair;
  else return NULL;
  
}

