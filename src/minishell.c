/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebouther <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/03/28 17:37:30 by ebouther          #+#    #+#             */
/*   Updated: 2016/03/30 01:00:18 by ebouther         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_get_in_env(char *search, char **env)
{
	int	i;
	char *ret;

	i = 0;
	ret = NULL;
	while (env[i])
		if ((ret = ft_strstr((const char *)env[i++], search)) != NULL)
			return (i - 1);
	return (-1);
}

static int	ft_open_home_dir(char **env)
{
	int	i;

	i = 0;
	if ((i = ft_get_in_env("HOME=", env)) != -1)
	{
		if (chdir((const char *)env[i] + 5) == -1)
			ft_printf("CHDIR Error\n");
		else
			return (0);
	}
	else
		ft_printf("$HOME is not set.\n");
	return (-1);
}

static void	ft_open_dir(char *dir, t_env *e)
{
	if (chdir((const char *)dir) == -1)
		ft_printf("CHDIR Error\n");
	else
	{
		if (e->last_dir != NULL)
			ft_strdel(&e->last_dir);
		e->last_dir = ft_strdup(dir); //Should set env OLDPWD
	}
}

static void	ft_change_directory(char **arg, char **env, t_env *e)
{
	int	i;
	//char directory[1024];

	i = -1;
	while (arg[++i])
		;
	if (i > 2)
		ft_printf("cd: string not in pwd: %s\n", arg[1]);
	else if (i > 1)
	{
		if (ft_strcmp(arg[1], "-") == 0)
		{
			if (e->last_dir != NULL)
				ft_open_dir(e->last_dir, e);
			else
				ft_printf("minishell: cd: OLDPWD not set\n"); //Well that's a lie, but who cares ?!
		}
		else if (ft_strcmp(arg[1], "~") == 0)
			ft_open_home_dir(env);
		else if (access(arg[1], R_OK) == 0)
			ft_open_dir(arg[1], e);
		else
			ft_printf("cd: cannot access to path.\n");
	}
	else
		ft_open_home_dir(env);
}

static char	**ft_get_user_input(char **env, t_env *e)
{
	char	**arg;
	char	*str;
	int		i;

	arg = NULL;
	if (get_next_line(1, &str))
	{
		if ((arg = ft_strsplit(str, ' ')) != NULL)
		{
			if (ft_strcmp(arg[0], "exit") == 0)
				exit(0);
			else if (ft_strcmp(arg[0], "cd") == 0)
				ft_change_directory(arg, env, e);
			else if (ft_strcmp(arg[0], "clear") == 0)
				ft_printf("\033[2J\033[1;1H");
			else
			{
				ft_strdel(&str);
				return (arg);
			}

			i = 0;
			while (arg[i])
				ft_strdel(arg + i++);
			free((void *)arg);
			arg = NULL;
		}
		ft_strdel(&str);
	}
	return (NULL);
}

static void	ft_find_and_exec_bin(char **input, char **env)
{
	char	*path;
	char	**split;
	int		i;
	int		n;

	
	if (execve(input[0], input, NULL) == -1
		&& (i = ft_get_in_env("PATH=", env)) != -1)
	{
		split = ft_strsplit(env[i], ':');
		i = 0;
		while (split[i] != '\0')
		{
			if (access(path = ft_strjoin_free(ft_strdup(split[i]),
							ft_strjoin_free(ft_strdup("/"),
								ft_strdup(input[0]))), X_OK) == 0)
			{
				if (execve(path, input, NULL) == -1)
					ft_printf("EXECVE ERROR.\n");
				ft_strdel(&path);
				return ;
			}
			ft_strdel(&path);
			i++;
		}
		
		n = 0;
		ft_printf("minishell: command not found:");
		while (input[n])
			ft_printf(" %s", input[n++]);
		ft_putchar('\n');
		
		n = 0;
		while (input[n])
			ft_strdel(input + n++);

		if (split != NULL)
		{
			n = 0;
			while (split[n])
				ft_strdel(split + n++);
		}
		exit(-1);
	}
}

int	main(int ac, char **av, char **env)
{
	pid_t	pid = -1;
	char	**input;
	int		i;
	t_env	e;
	
	e.last_dir = NULL;
	(void)ac;
	(void)av;

	while (42)
	{
		ft_printf("$> ");
		if ((input = ft_get_user_input(env, &e)) != NULL)
			pid = fork();
		if (pid == 0) //child process
		{
			ft_find_and_exec_bin(input, env);
		}
		else if (pid > 0) //father process
			wait(NULL);
		i = 0;
		if (input != NULL)
		{
			while (input[i])
				ft_strdel(input + i);
			free((void *)input);
			input = NULL;
		}
	}
	return (0);
}
